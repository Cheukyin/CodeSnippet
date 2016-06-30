#include "kernel.h"
#include "lib.h"

// k-hardware.c
//
//    Grody functions for interacting with x86 hardware.


// hardware_init
//    Initialize hardware. Calls other functions bellow.

static void segments_init(void);
static void interrupt_init(void);
static void virtual_memory_init(void);

void hardware_init(void) {
    segments_init();
    interrupt_init();
    virtual_memory_init();
}


// segments_init
//    Set up segment registers and interrupt descriptor table.
//
//    The segment registers distinguish the kernel from applications:
//    the kernel runs with segments SEGSEL_KERN_CODE and SEGSEL_KERN_DATA,
//    and applications with SEGSEL_APP_CODE and SEGSEL_APP_DATA.
//    The kernel segment runs with full privilege (level 0), but application
//    segments run with less privilege (level 3).
//
//    The interrupt descriptor table tells the processor where to jump
//    when an interrupt or exception happens. See k-interrupt.S.
//
//    The taskstate_t, segmentdescriptor_t, and pseduodescriptor_t types
//    are defined by the x86 hardware.

// Segment selectors
#define SEGSEL_KERN_CODE        0x8             // kernel code segment
#define SEGSEL_KERN_DATA        0x10            // kernel data segment
                                                // (SEE ALSO k-interrupt.S)
#define SEGSEL_APP_CODE         0x18            // application code segment
#define SEGSEL_APP_DATA         0x20            // application data segment
#define SEGSEL_TASKSTATE        0x28            // task state segment

// The task descriptor defines the state the processor should set up
// when taking an interrupt.
static x86_taskstate kernel_task_descriptor;

// Segments
static x86_segmentdescriptor segments[] = {
        SEG_NULL,                               // ignored
        SEG(STA_X | STA_R, 0, 0xFFFFFFFF, 0),   // SEGSEL_KERN_CODE
        SEG(STA_W, 0, 0xFFFFFFFF, 0),           // SEGSEL_KERN_DATA
        SEG(STA_X | STA_R, 0, 0xFFFFFFFF, 3),   // SEGSEL_APP_CODE
        SEG(STA_W, 0, 0xFFFFFFFF, 3),           // SEGSEL_APP_DATA
        SEG_NULL /* defined below */            // SEGSEL_TASKSTATE
};
static x86_pseudodescriptor global_descriptor_table = {
        sizeof(segments) - 1,
        (uintptr_t) segments
};

// Interrupt descriptors
static x86_gatedescriptor interrupt_descriptors[256];
static x86_pseudodescriptor interrupt_descriptor_table = {
        sizeof(interrupt_descriptors) - 1,
        (uintptr_t) interrupt_descriptors
};

// Particular interrupt handler routines
extern void (*sys_int_handlers[])(void);
extern void default_int_handler(void);
extern void gpf_int_handler(void);
extern void pagefault_int_handler(void);
extern void timer_int_handler(void);

void segments_init(void) {
    // Set task state segment
    segments[SEGSEL_TASKSTATE >> 3]
        = SEG16(STS_T32A, (uint32_t) &kernel_task_descriptor,
                sizeof(x86_taskstate), 0);
    segments[SEGSEL_TASKSTATE >> 3].sd_s = 0;

    // Set up kernel task descriptor, so we can receive interrupts
    kernel_task_descriptor.ts_esp0 = KERNEL_STACK_TOP;
    kernel_task_descriptor.ts_ss0 = SEGSEL_KERN_DATA;

    // Set up interrupt descriptor table.
    // Most interrupts are effectively ignored
    memset(interrupt_descriptors, 0, sizeof(interrupt_descriptors));
    for (int i = 16;
         i < sizeof(interrupt_descriptors) / sizeof(x86_gatedescriptor);
         ++i)
        SETGATE(interrupt_descriptors[i], 0,
                SEGSEL_KERN_CODE, default_int_handler, 0);

    // Timer interrupt
    SETGATE(interrupt_descriptors[INT_TIMER], 0,
            SEGSEL_KERN_CODE, timer_int_handler, 0);

    // GPF and page fault
    SETGATE(interrupt_descriptors[INT_GPF], 0,
            SEGSEL_KERN_CODE, gpf_int_handler, 0);
    SETGATE(interrupt_descriptors[INT_PAGEFAULT], 0,
            SEGSEL_KERN_CODE, pagefault_int_handler, 0);

    // System calls get special handling.
    // Note that the last argument is '3'.  This means that unprivileged
    // (level-3) applications may generate these interrupts.
    for (int i = INT_SYS; i < INT_SYS + 16; ++i)
        SETGATE(interrupt_descriptors[i], 0,
                SEGSEL_KERN_CODE, sys_int_handlers[i - INT_SYS], 3);

    // Reload segment pointers
    asm volatile("lgdt %0\n\t"
                 "ltr %1\n\t"
                 "lidt %2"
                 :
                 : "m" (global_descriptor_table),
                   "r" ((uint16_t) SEGSEL_TASKSTATE),
                   "m" (interrupt_descriptor_table));
}


// interrupt_init
//    Set up the interrupt controller (Intel part number 8259A).
//
//    Each interrupt controller supports up to 8 different kinds of interrupt.
//    The first x86s supported only one controller; this was too few, so modern
//    x86 machines can have more than one controller, a master and some slaves.
//    Much hoop-jumping is required to get the controllers to communicate!
//
//    Note: "IRQ" stands for "Interrupt ReQuest line", and stands for an
//    interrupt number.

#define MAX_IRQS        16      // Number of IRQs

// I/O Addresses of the two 8259A programmable interrupt controllers
#define IO_PIC1         0x20    // Master (IRQs 0-7)
#define IO_PIC2         0xA0    // Slave (IRQs 8-15)

#define IRQ_SLAVE       2       // IRQ at which slave connects to master

// Timer-related constants
#define IO_TIMER1       0x040           /* 8253 Timer #1 */
#define TIMER_MODE      (IO_TIMER1 + 3) /* timer mode port */
#define   TIMER_SEL0    0x00            /* select counter 0 */
#define   TIMER_RATEGEN 0x04            /* mode 2, rate generator */
#define   TIMER_16BIT   0x30            /* r/w counter 16 bits, LSB first */

// Timer frequency: (TIMER_FREQ/freq) generates a frequency of 'freq' Hz.
#define TIMER_FREQ      1193182
#define TIMER_DIV(x)    ((TIMER_FREQ+(x)/2)/(x))

static uint16_t interrupts_enabled;

static void interrupt_mask(void) {
    uint16_t masked = ~interrupts_enabled;
    outb(IO_PIC1+1, masked & 0xFF);
    outb(IO_PIC2+1, (masked >> 8) & 0xFF);
}

void interrupt_init(void) {
    // mask all interrupts
    interrupts_enabled = 0;
    interrupt_mask();

    /* Set up master (8259A-1) */
    // ICW1:  0001g0hi
    //    g:  0 = edge triggering (1 = level triggering)
    //    h:  0 = cascaded PICs (1 = master only)
    //    i:  1 = ICW4 required (0 = no ICW4)
    outb(IO_PIC1, 0x11);

    // ICW2:  Trap offset. Interrupt 0 will cause trap INT_HARDWARE.
    outb(IO_PIC1+1, INT_HARDWARE);

    // ICW3:  On master PIC, bit mask of IR lines connected to slave PICs;
    //        on slave PIC, IR line at which slave connects to master (0-8)
    outb(IO_PIC1+1, 1<<IRQ_SLAVE);

    // ICW4:  000nbmap
    //    n:  1 = special fully nested mode
    //    b:  1 = buffered mode
    //    m:  0 = slave PIC, 1 = master PIC
    //    (ignored when b is 0, as the master/slave role
    //    can be hardwired).
    //    a:  1 = Automatic EOI mode
    //    p:  0 = MCS-80/85 mode, 1 = intel x86 mode
    outb(IO_PIC1+1, 0x3);

    /* Set up slave (8259A-2) */
    outb(IO_PIC2, 0x11);                        // ICW1
    outb(IO_PIC2+1, INT_HARDWARE + 8);  // ICW2
    outb(IO_PIC2+1, IRQ_SLAVE);         // ICW3
    // NB Automatic EOI mode doesn't tend to work on the slave.
    // Linux source code says it's "to be investigated".
    outb(IO_PIC2+1, 0x01);                      // ICW4

    // OCW3:  0ef01prs
    //   ef:  0x = NOP, 10 = clear specific mask, 11 = set specific mask
    //    p:  0 = no polling, 1 = polling mode
    //   rs:  0x = NOP, 10 = read IRR, 11 = read ISR
    outb(IO_PIC1, 0x68);             /* clear specific mask */
    outb(IO_PIC1, 0x0a);             /* read IRR by default */

    outb(IO_PIC2, 0x68);               /* OCW3 */
    outb(IO_PIC2, 0x0a);               /* OCW3 */

    // re-disable interrupts
    interrupt_mask();
}


// timer_init(rate)
//    Set the timer interrupt to fire `rate` times a second. Disables the
//    timer interrupt if `rate <= 0`.

void timer_init(int rate) {
    if (rate > 0) {
        outb(TIMER_MODE, TIMER_SEL0 | TIMER_RATEGEN | TIMER_16BIT);
        outb(IO_TIMER1, TIMER_DIV(rate) % 256);
        outb(IO_TIMER1, TIMER_DIV(rate) / 256);
        interrupts_enabled |= 1 << (INT_TIMER - INT_HARDWARE);
    } else
        interrupts_enabled &= ~(1 << (INT_TIMER - INT_HARDWARE));
    interrupt_mask();
}


// virtual_memory_init
//    Initialize the virtual memory system, including an initial page table
//    `kernel_pagetable`.

static x86_pagetable kernel_pagetable_memory;
static x86_pagetable kernel_level2_pagetable;
x86_pagetable* kernel_pagetable;

void virtual_memory_init(void) {
    kernel_pagetable = &kernel_pagetable_memory;
    memset(kernel_pagetable, 0, sizeof(x86_pagetable));
    kernel_pagetable->entry[0] = (x86_pageentry_t) &kernel_level2_pagetable
            | PTE_P | PTE_W | PTE_U;

    virtual_memory_map(kernel_pagetable, (uintptr_t) 0, (uintptr_t) 0,
                       MEMSIZE_PHYSICAL, PTE_P | PTE_W | PTE_U);

    // Use special instructions to initialize paged virtual memory.
    lcr3((uintptr_t) kernel_pagetable);
    uint32_t cr0 = rcr0();
    cr0 |= CR0_PE | CR0_PG | CR0_AM | CR0_WP | CR0_NE | CR0_TS
        | CR0_EM | CR0_MP;
    cr0 &= ~(CR0_TS | CR0_EM);
    lcr0(cr0);
}


// virtual_memory_map(pagetable, va, pa, sz, perm)
//    Map virtual address range `[va, va+sz)` in `pagetable`.
//    When `X >= 0 && X < sz`, the new pagetable will map virtual address
//    `va+X` to physical address `pa+X` with permissions `perm`.
//
//    Preconditions:
//    * `va`, `pa`, and `sz` must be multiples of PAGESIZE (4096).
//    * The level-2 pagetables referenced by the virtual address range
//      must exist and be writable (e.g., `va + sz <= MEMSIZE_VIRTUAL`).
//
//    Typically `perm` is a combination of `PTE_P` (the memory is Present),
//    `PTE_W` (the memory is Writable), and `PTE_U` (the memory may be
//    accessed by Unprivileged applications). If `!(perm & PTE_P)`, `pa` is
//    ignored.

void virtual_memory_map(x86_pagetable* pagetable, uintptr_t va, uintptr_t pa,
                        size_t sz, int perm) {
    assert(va % PAGESIZE == 0); // virtual address is page-aligned
    assert(sz % PAGESIZE == 0); // size is a multiple of PAGESIZE
    assert(va + sz >= va || va + sz == 0); // va range does not wrap
    if (perm & PTE_P) {
        assert(pa % PAGESIZE == 0); // physical addr is page-aligned
        assert(pa + sz >= pa);      // physical address range does not wrap
        assert(pa + sz <= MEMSIZE_PHYSICAL); // physical addresses exist
    }
    assert(perm >= 0 && perm < 0x1000); // `perm` makes sense
    assert((uintptr_t) pagetable % PAGESIZE == 0); // `pagetable` page-aligned

    int l1pageindex = -1;
    x86_pagetable* l2pagetable = 0;
    for (; sz != 0; va += PAGESIZE, pa += PAGESIZE, sz -= PAGESIZE) {
        if (L1PAGEINDEX(va) != l1pageindex) {
            l1pageindex = L1PAGEINDEX(va);
            x86_pageentry_t l1pte = pagetable->entry[l1pageindex];
            assert(l1pte & PTE_P);      // level-2 pagetable must exist
            assert(PTE_ADDR(l1pte) < MEMSIZE_VIRTUAL); // at a sensible address
            if (perm & PTE_W)           // if requester wants PTE_W,
                assert(l1pte & PTE_W);  //   level-1 entry must allow PTE_W
            if (perm & PTE_U)           // if requester wants PTE_U,
                assert(l1pte & PTE_U);  //   level-1 entry must allow PTE_U
            l2pagetable = (x86_pagetable*) PTE_ADDR(l1pte);
        }
        if (perm & PTE_P)
            l2pagetable->entry[L2PAGEINDEX(va)] = pa | perm;
        else
            l2pagetable->entry[L2PAGEINDEX(va)] = perm;
    }
}


// virtual_memory_lookup(pagetable, va)
//    Returns information about the mapping of the virtual address `va` in
//    `pagetable`. The information is returned as a `vamapping` object.

vamapping virtual_memory_lookup(x86_pagetable* pagetable, uintptr_t va) {
    x86_pageentry_t pe = pagetable->entry[L1PAGEINDEX(va)];
    if (pe & PTE_P) {
        x86_pagetable* l2pagetable = (x86_pagetable*) PTE_ADDR(pe);
        pe = l2pagetable->entry[L2PAGEINDEX(va)] & (pe | ~(PTE_W | PTE_U));
    }
    if (pe & PTE_P)
        return (vamapping) { PAGENUMBER(pe),
                             PTE_ADDR(pe) + PAGEOFFSET(va),
                             PTE_FLAGS(pe) };
    else
        return (vamapping) { -1, (uintptr_t) -1, 0 };
}


// set_pagetable
//    Change page directory. lcr3() is the hardware instruction;
//    set_pagetable() additionally checks that important kernel procedures are
//    mappable in `pagetable`, and calls panic() if they aren't.

void set_pagetable(x86_pagetable* pagetable) {
    assert(PAGEOFFSET(pagetable) == 0); // must be page aligned
    assert(virtual_memory_lookup(pagetable, (uintptr_t) default_int_handler).pa
           == (uintptr_t) default_int_handler);
    assert(virtual_memory_lookup(kernel_pagetable, (uintptr_t) pagetable).pa
           == (uintptr_t) pagetable);
    assert(virtual_memory_lookup(pagetable, (uintptr_t) kernel_pagetable).pa
           == (uintptr_t) kernel_pagetable);
    assert(virtual_memory_lookup(pagetable, (uintptr_t) virtual_memory_map).pa
           == (uintptr_t) virtual_memory_map);
    lcr3((uintptr_t) pagetable);
}


// physical_memory_isreserved(pa)
//    Returns non-zero iff `pa` is a reserved physical address.

#define IOPHYSMEM       0x000A0000
#define EXTPHYSMEM      0x00100000

int physical_memory_isreserved(uintptr_t pa) {
    return pa == 0 || (pa >= IOPHYSMEM && pa < EXTPHYSMEM);
}


// pci_make_configaddr(bus, slot, func)
//    Construct a PCI configuration space address from parts.

static int pci_make_configaddr(int bus, int slot, int func) {
    return (bus << 16) | (slot << 11) | (func << 8);
}


// pci_config_readl(bus, slot, func, offset)
//    Read a 32-bit word in PCI configuration space.

#define PCI_HOST_BRIDGE_CONFIG_ADDR 0xCF8
#define PCI_HOST_BRIDGE_CONFIG_DATA 0xCFC

static uint32_t pci_config_readl(int configaddr, int offset) {
    outl(PCI_HOST_BRIDGE_CONFIG_ADDR, 0x80000000 | configaddr | offset);
    return inl(PCI_HOST_BRIDGE_CONFIG_DATA);
}


// pci_find_device
//    Search for a PCI device matching `vendor` and `device`. Return
//    the config base address or -1 if no device was found.

static int pci_find_device(int vendor, int device) {
    for (int bus = 0; bus != 256; ++bus)
        for (int slot = 0; slot != 32; ++slot)
            for (int func = 0; func != 8; ++func) {
                int configaddr = pci_make_configaddr(bus, slot, func);
                uint32_t vendor_device = pci_config_readl(configaddr, 0);
                if (vendor_device == (vendor | (device << 16)))
                    return configaddr;
                else if (vendor_device == (uint32_t) -1 && func == 0)
                    break;
            }
    return -1;
}

// poweroff
//    Turn off the virtual machine. This requires finding a PCI device
//    that speaks ACPI; QEMU emulates a PIIX4 Power Management Controller.

#define PCI_VENDOR_ID_INTEL     0x8086
#define PCI_DEVICE_ID_PIIX4     0x7113

void poweroff(void) {
    int configaddr = pci_find_device(PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_PIIX4);
    if (configaddr >= 0) {
        // Read I/O base register from controller's PCI configuration space.
        int pm_io_base = pci_config_readl(configaddr, 0x40) & 0xFFC0;
        // Write `suspend enable` to the power management control register.
        outw(pm_io_base + 4, 0x2000);
    }
    // No PIIX4; spin.
    console_printf(CPOS(24, 0), 0xC000, "Cannot power off!\n");
 spinloop: goto spinloop;
}


// reboot
//    Reboot the virtual machine.

void reboot(void) {
    outb(0x92, 3);
 spinloop: goto spinloop;
}


// process_init(p, flags)
//    Initialize special-purpose registers for process `p`.

void process_init(proc* p, int flags) {
    memset(&p->p_registers, 0, sizeof(p->p_registers));
    p->p_registers.reg_cs = SEGSEL_APP_CODE | 3;
    p->p_registers.reg_ds = SEGSEL_APP_DATA | 3;
    p->p_registers.reg_es = SEGSEL_APP_DATA | 3;
    p->p_registers.reg_ss = SEGSEL_APP_DATA | 3;
    p->p_registers.reg_eflags = EFLAGS_IF;

    if (flags & PROCINIT_ALLOW_PROGRAMMED_IO)
        p->p_registers.reg_eflags |= EFLAGS_IOPL_3;
    if (flags & PROCINIT_DISABLE_INTERRUPTS)
        p->p_registers.reg_eflags &= ~EFLAGS_IF;
}


// console_show_cursor(cpos)
//    Move the console cursor to position `cpos`, which should be between 0
//    and 80 * 25.

void console_show_cursor(int cpos) {
    if (cpos < 0 || cpos > CONSOLE_ROWS * CONSOLE_COLUMNS)
        cpos = 0;
    outb(0x3D4, 14);
    outb(0x3D5, cpos / 256);
    outb(0x3D4, 15);
    outb(0x3D5, cpos % 256);
}



// keyboard_readc
//    Read a character from the keyboard. Returns -1 if there is no character
//    to read, and 0 if no real key press was registered but you should call
//    keyboard_readc() again (e.g. the user pressed a SHIFT key). Otherwise
//    returns either an ASCII character code or one of the special characters
//    listed in kernel.h.

// Unfortunately mapping PC key codes to ASCII takes a lot of work.

#define MOD_SHIFT       (1 << 0)
#define MOD_CONTROL     (1 << 1)
#define MOD_CAPSLOCK    (1 << 3)

#define KEY_SHIFT       0372
#define KEY_CONTROL     0373
#define KEY_ALT         0374
#define KEY_CAPSLOCK    0375
#define KEY_NUMLOCK     0376
#define KEY_SCROLLLOCK  0377

#define CKEY(cn)        0x80 + cn

static const uint8_t keymap[256] = {
    /*0x00*/ 0, 033, CKEY(0), CKEY(1), CKEY(2), CKEY(3), CKEY(4), CKEY(5),
        CKEY(6), CKEY(7), CKEY(8), CKEY(9), CKEY(10), CKEY(11), '\b', '\t',
    /*0x10*/ 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i',
        'o', 'p', CKEY(12), CKEY(13), CKEY(14), KEY_CONTROL, 'a', 's',
    /*0x20*/ 'd', 'f', 'g', 'h', 'j', 'k', 'l', CKEY(15),
        CKEY(16), CKEY(17), KEY_SHIFT, CKEY(18), 'z', 'x', 'c', 'v',
    /*0x30*/ 'b', 'n', 'm', CKEY(19), CKEY(20), CKEY(21), KEY_SHIFT, '*',
        KEY_ALT, ' ', KEY_CAPSLOCK, 0, 0, 0, 0, 0,
    /*0x40*/ 0, 0, 0, 0, 0, KEY_NUMLOCK, KEY_SCROLLLOCK, '7',
        '8', '9', '-', '4', '5', '6', '+', '1',
    /*0x50*/ '2', '3', '0', '.', 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x60*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x70*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x80*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0x90*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, CKEY(14), KEY_CONTROL, 0, 0,
    /*0xA0*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0xB0*/ 0, 0, 0, 0, 0, '/', 0, 0,  KEY_ALT, 0, 0, 0, 0, 0, 0, 0,
    /*0xC0*/ 0, 0, 0, 0, 0, 0, 0, KEY_HOME,
        KEY_UP, KEY_PAGEUP, 0, KEY_LEFT, 0, KEY_RIGHT, 0, KEY_END,
    /*0xD0*/ KEY_DOWN, KEY_PAGEDOWN, KEY_INSERT, KEY_DELETE, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0,
    /*0xE0*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0,
    /*0xF0*/ 0, 0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0
};

static const struct keyboard_key {
    uint8_t map[4];
} complex_keymap[] = {
    /*CKEY(0)*/ {{'1', '!', 0, 0}},  /*CKEY(1)*/ {{'2', '@', 0, 0}},
    /*CKEY(2)*/ {{'3', '#', 0, 0}},  /*CKEY(3)*/ {{'4', '$', 0, 0}},
    /*CKEY(4)*/ {{'5', '%', 0, 0}},  /*CKEY(5)*/ {{'6', '^', 0, 036}},
    /*CKEY(6)*/ {{'7', '&', 0, 0}},  /*CKEY(7)*/ {{'8', '*', 0, 0}},
    /*CKEY(8)*/ {{'9', '(', 0, 0}},  /*CKEY(9)*/ {{'0', ')', 0, 0}},
    /*CKEY(10)*/ {{'-', '_', 0, 037}},  /*CKEY(11)*/ {{'=', '+', 0, 0}},
    /*CKEY(12)*/ {{'[', '{', 033, 0}},  /*CKEY(13)*/ {{']', '}', 035, 0}},
    /*CKEY(14)*/ {{'\n', '\n', '\r', '\r'}},
    /*CKEY(15)*/ {{';', ':', 0, 0}},
    /*CKEY(16)*/ {{'\'', '"', 0, 0}},  /*CKEY(17)*/ {{'`', '~', 0, 0}},
    /*CKEY(18)*/ {{'\\', '|', 034, 0}},  /*CKEY(19)*/ {{',', '<', 0, 0}},
    /*CKEY(20)*/ {{'.', '>', 0, 0}},  /*CKEY(21)*/ {{'/', '?', 0, 0}}
};

int keyboard_readc(void) {
    static uint8_t modifiers;
    static uint8_t last_escape;

    if ((inb(KEYBOARD_STATUSREG) & KEYBOARD_STATUS_READY) == 0)
        return -1;

    uint8_t data = inb(KEYBOARD_DATAREG);
    uint8_t escape = last_escape;
    last_escape = 0;

    if (data == 0xE0) {         // mode shift
        last_escape = 0x80;
        return 0;
    } else if (data & 0x80) {   // key release: matters only for modifier keys
        int ch = keymap[(data & 0x7F) | escape];
        if (ch >= KEY_SHIFT && ch < KEY_CAPSLOCK)
            modifiers &= ~(1 << (ch - KEY_SHIFT));
        return 0;
    }

    int ch = (unsigned char) keymap[data | escape];

    if (ch >= 'a' && ch <= 'z') {
        if (modifiers & MOD_CONTROL)
            ch -= 0x60;
        else if (!(modifiers & MOD_SHIFT) != !(modifiers & MOD_CAPSLOCK))
            ch -= 0x20;
    } else if (ch >= KEY_CAPSLOCK) {
        modifiers ^= 1 << (ch - KEY_SHIFT);
        ch = 0;
    } else if (ch >= KEY_SHIFT) {
        modifiers |= 1 << (ch - KEY_SHIFT);
        ch = 0;
    } else if (ch >= CKEY(0) && ch <= CKEY(21))
        ch = complex_keymap[ch - CKEY(0)].map[modifiers & 3];
    else if (ch < 0x80 && (modifiers & MOD_CONTROL))
        ch = 0;

    return ch;
}


// log_printf, log_vprintf
//    Print debugging messages to the host's `log.txt` file. We run QEMU
//    so that messages written to the QEMU "parallel port" end up in `log.txt`.

#define IO_PARALLEL1_DATA       0x378
#define IO_PARALLEL1_STATUS     0x379
# define IO_PARALLEL_STATUS_BUSY        0x80
#define IO_PARALLEL1_CONTROL    0x37A
# define IO_PARALLEL_CONTROL_SELECT     0x08
# define IO_PARALLEL_CONTROL_INIT       0x04
# define IO_PARALLEL_CONTROL_STROBE     0x01

static void delay(void) {
    (void) inb(0x84);
    (void) inb(0x84);
    (void) inb(0x84);
    (void) inb(0x84);
}

static void parallel_port_putc(printer* p, unsigned char c, int color) {
    static int initialized;
    (void) p, (void) color;
    if (!initialized) {
        outb(IO_PARALLEL1_CONTROL, 0);
        initialized = 1;
    }

    for (int i = 0;
         i < 12800 && (inb(IO_PARALLEL1_STATUS) & IO_PARALLEL_STATUS_BUSY) == 0;
         ++i)
        delay();
    outb(IO_PARALLEL1_DATA, c);
    outb(IO_PARALLEL1_CONTROL, IO_PARALLEL_CONTROL_SELECT
         | IO_PARALLEL_CONTROL_INIT | IO_PARALLEL_CONTROL_STROBE);
    outb(IO_PARALLEL1_CONTROL, IO_PARALLEL_CONTROL_SELECT
         | IO_PARALLEL_CONTROL_INIT);
}

void log_vprintf(const char* format, va_list val) {
    printer p;
    p.putc = parallel_port_putc;
    printer_vprintf(&p, 0, format, val);
}

void log_printf(const char* format, ...) {
    va_list val;
    va_start(val, format);
    log_vprintf(format, val);
    va_end(val);
}


// check_keyboard
//    Check for the user typing a control key. 'a', 'f', and 'e' cause a soft
//    reboot where the kernel runs the allocator programs, "fork", or
//    "forkexit", respectively. Control-C or 'q' exit the virtual machine.

void check_keyboard(void) {
    int c = keyboard_readc();
    if (c == 'a' || c == 'f' || c == 'e') {
        // The soft reboot process doesn't modify memory, so it's
        // safe to pass `multiboot_info` on the kernel stack, even
        // though it will get overwritten as the kernel runs.
        uint32_t multiboot_info[5];
        multiboot_info[0] = 4;
        multiboot_info[4] = (uint32_t) (c == 'a' ? "allocator"
                                        : (c == 'e' ? "forkexit" : "fork"));
        asm volatile("movl $0x2BADB002, %%eax; jmp multiboot_start"
                     : : "b" (multiboot_info) : "memory");
    } else if (c == 0x03 || c == 'q')
        poweroff();
}


// fail
//    Loop until user presses Control-C, then poweroff.

static void fail(void) __attribute__((noreturn));
static void fail(void) {
    while (1)
        check_keyboard();
}


// panic, assert_fail
//    Use console_printf() to print a failure message and then wait for
//    control-C. Also write the failure message to the log.

void panic(const char* format, ...) {
    va_list val;
    va_start(val, format);

    if (format) {
        // Print panic message to both the screen and the log
        int cpos = console_printf(CPOS(23, 0), 0xC000, "PANIC: ");
        log_printf("PANIC: ");

        cpos = console_vprintf(cpos, 0xC000, format, val);
        log_vprintf(format, val);

        if (CCOL(cpos)) {
            cpos = console_printf(cpos, 0xC000, "\n");
            log_printf("\n");
        }
    } else {
        (void) console_printf(CPOS(23, 0), 0xC000, "PANIC");
        log_printf("PANIC\n");
    }

    va_end(val);
    fail();
}

void assert_fail(const char* file, int line, const char* msg) {
    panic("%s:%d: assertion '%s' failed\n", file, line, msg);
}
