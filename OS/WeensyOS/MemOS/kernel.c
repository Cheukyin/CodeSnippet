#include "kernel.h"
#include "lib.h"

// kernel.c
//
//    This is the kernel.


// INITIAL PHYSICAL MEMORY LAYOUT
//
//  +-------------- Base Memory --------------+
//  v                                         v
// +-----+--------------------+----------------+--------------------+---------/
// |     | Kernel      Kernel |       :    I/O | App 1        App 1 | App 2
// |     | Code + Data  Stack |  ...  : Memory | Code + Data  Stack | Code ...
// +-----+--------------------+----------------+--------------------+---------/
// 0  0x40000              0x80000 0xA0000 0x100000             0x140000
//                                             ^
//                                             | \___ PROC_SIZE ___/
//                                      PROC_START_ADDR

#define PROC_SIZE 0x40000       // initial state only

static proc processes[NPROC];   // array of process descriptors
                                // Note that `processes[0]` is never used.
proc* current;                  // pointer to currently executing proc

#define HZ 100                  // timer interrupt frequency (interrupts/sec)
static unsigned ticks;          // # timer interrupts so far

void schedule(void);
void run(proc* p) __attribute__((noreturn));


// PAGEINFO
//
//    The pageinfo[] array keeps track of information about each physical page.
//    There is one entry per physical page.
//    `pageinfo[pn]` holds the information for physical page number `pn`.
//    You can get a physical page number from a physical address `pa` using
//    `PAGENUMBER(pa)`. (This also works for page table entries.)
//    To change a physical page number `pn` into a physical address, use
//    `PAGEADDRESS(pn)`.
//
//    pageinfo[pn].refcount is the number of times physical page `pn` is
//      currently referenced. 0 means it's free.
//    pageinfo[pn].owner is a constant indicating who owns the page.
//      PO_KERNEL means the kernel, PO_RESERVED means reserved memory (such
//      as the console), and a number >=0 means that process ID.
//
//    pageinfo_init() sets up the initial pageinfo[] state.

typedef struct physical_pageinfo {
    int8_t owner;
    int8_t refcount;
} physical_pageinfo;

static physical_pageinfo pageinfo[PAGENUMBER(MEMSIZE_PHYSICAL)];

typedef enum pageowner {
    PO_FREE = 0,                // this page is free
    PO_RESERVED = -1,           // this page is reserved memory
    PO_KERNEL = -2              // this page is used by the kernel
} pageowner_t;

static void pageinfo_init(void);


// Memory functions

void virtual_memory_check(void);
void memshow_physical(void);
void memshow_virtual(x86_pagetable* pagetable, const char* name);
void memshow_virtual_animate(void);
int fork(void);


// kernel(command)
//    Initialize the hardware and processes and start running. The `command`
//    string is an optional string passed from the boot loader.

static void process_setup(pid_t pid, int program_number);

static uintptr_t find_free_physic_page(pageowner_t owner);
static x86_pagetable* copy_pagetable(x86_pagetable* pagedir, int8_t owner);

void kernel(const char* command) {
    hardware_init();
    pageinfo_init();
    console_clear();
#if TICKTYPE == 1
    ticks = 1;
#else
    timer_init(HZ);
#endif

    memset(processes, 0, sizeof(processes));
    for (pid_t i = 0; i < NPROC; i++) {
        processes[i].p_pid = i;
        processes[i].p_state = P_FREE;
    }

    // Exercise 1: your code here
    //  
    //  Note that by the time the code gets to this point, the virtual
    //  memory mapping is set up to be the identity mapping (as noted in
    //  the lab description). (Use your code navigation skills to search
    //  for previous invocations of virtual_memory_map.) Furthermore,
    //  all processes have access to all memory.
    //
    //  Your job is to use virtual_memory_map to 
    //    (1) _restrict_ processes from accessing virtual addresses
    //    that "belong" to the kernel, and
    //    (2) allow processes to access the single page beginning at the
    //    address 'console'.
    //
    // Hints:
    //  * Call virtual_memory_map twice. 
    //  * For (1), above, the part of each process's address space
    //    that "belongs" to the kernel is virtual addresses
    //    [0,PROC_START_ADDR). This is indicated in the lab description,
    //    and we repeat it in this hint.

    virtual_memory_map(kernel_pagetable, 0, 0, PROC_START_ADDR, PTE_P | PTE_W);
    virtual_memory_map(kernel_pagetable, (uintptr_t)console, (uintptr_t)console, PAGESIZE, PTE_P | PTE_W | PTE_U);

    if (command && strcmp(command, "fork") == 0)
        process_setup(1, 4);
    else if (command && strcmp(command, "forkexit") == 0)
        process_setup(1, 5);
    else
        for (pid_t i = 1; i <= 4; ++i)
            process_setup(i, i - 1);

    // Switch to the first process using run()
    run(&processes[1]);
}


// process_setup(pid, program_number)
//    Load application program `program_number` as process number `pid`.
//    This loads the application's code and data into memory, sets its
//    %eip and %esp, gives it a stack page, and marks it as runnable.

void process_setup(pid_t pid, int program_number) {
    process_init(&processes[pid], 0);

    // Exercise 2: your code here
    processes[pid].p_pagetable = copy_pagetable(kernel_pagetable, pid);
    x86_pagetable* pid_pagetable = (x86_pagetable*)PTE_ADDR(processes[pid].p_pagetable->entry[0]);
    memset(&pid_pagetable->entry[PAGENUMBER(PROC_START_ADDR)], 0,
           PAGESIZE - sizeof(x86_pageentry_t) * PAGENUMBER(PROC_START_ADDR));

    /* processes[pid].p_pagetable = kernel_pagetable; */
    /* ++pageinfo[PAGENUMBER(kernel_pagetable)].refcount; */
    int r = program_load(&processes[pid], program_number);
    assert(r >= 0);

    // Exercise 4: your code here
    processes[pid].p_registers.reg_esp = PROC_START_ADDR + PROC_SIZE * pid;
    uintptr_t stack_page = processes[pid].p_registers.reg_esp - PAGESIZE;
    physical_page_alloc(stack_page, pid);
    virtual_memory_map(processes[pid].p_pagetable, stack_page, stack_page,
                       PAGESIZE, PTE_P|PTE_W|PTE_U);
    processes[pid].p_state = P_RUNNABLE;
}


static uintptr_t find_free_physic_page(pageowner_t owner)
{
    uintptr_t i;
    for(i=0; i < PAGENUMBER(MEMSIZE_PHYSICAL); i++)
    {
        if(pageinfo[i].owner == PO_FREE)
        {
            pageinfo[i].refcount = 1;
            pageinfo[i].owner = owner;
            return PAGEADDRESS(i);
        }
    }

    panic("No free Page!");
}

static x86_pagetable* copy_pagetable(x86_pagetable* pagedir, int8_t owner)
{
    x86_pagetable* newpagedir = (x86_pagetable*)find_free_physic_page(owner);
    x86_pagetable* newpagetable = (x86_pagetable*)find_free_physic_page(owner);

    memset(newpagedir->entry, 0, PAGESIZE);
    newpagedir->entry[0] = (x86_pageentry_t)newpagetable | PTE_P | PTE_W | PTE_U;

    memcpy(newpagetable->entry, (void*)PTE_ADDR(pagedir->entry[0]), PAGESIZE);

    return newpagedir;
}


// physical_page_alloc(addr, owner)
//    The purpose of this function is to allocate a physical page 
//    (with physical address "addr" to the given owner). This function
//    fails if the requested physical page was in fact already allocated.
//    Returns -1 on failure and 0 on success. Used by the program loader.

int physical_page_alloc(uintptr_t addr, int8_t owner) {
    if ((addr & 0xFFF) != 0
        || addr >= MEMSIZE_PHYSICAL
        || pageinfo[PAGENUMBER(addr)].refcount != 0)
        return -1;
    else {
        pageinfo[PAGENUMBER(addr)].refcount = 1;
        pageinfo[PAGENUMBER(addr)].owner = owner;
        return 0;
    }
}


// exception(reg)
//    Exception handler (for interrupts, traps, and faults).
//
//    The register values from exception time are stored in `reg`.
//    The processor responds to an exception by saving application state on
//    the kernel's stack, then jumping to kernel assembly code (in
//    k-exception.S). That code saves more registers on the kernel's stack,
//    then calls exception().
//
//    Note that hardware interrupts are disabled whenever the kernel is running.

void exception(x86_registers* reg) {
    // The next line copies the saved registers into the
    // process descriptor of the `current` process.
    current->p_registers = *reg;
    // Always use the kernel's page table.
    set_pagetable(kernel_pagetable);

    // It can be useful to log events using `log_printf`.
    // Events logged this way are stored in the host's `log.txt` file.
    /*log_printf("proc %d: exception %d\n", current->p_pid, reg->reg_intno);*/

    // Show the current cursor location and memory state
    // (unless this is a kernel fault).
    console_show_cursor(cursorpos);
    if (reg->reg_intno != INT_PAGEFAULT || (reg->reg_err & PFERR_USER)) {
        virtual_memory_check();
        memshow_physical();
        memshow_virtual_animate();
    }

    // If Control-C was typed, exit the virtual machine.
    check_keyboard();


    // Actually handle the exception.
    switch (reg->reg_intno) {

    case INT_SYS_PANIC:
        panic(NULL);
        break;                  // will not be reached

    case INT_SYS_GETPID:
        current->p_registers.reg_eax = current->p_pid;
        break;

    case INT_SYS_YIELD:
        schedule();
        break;                  /* will not be reached */

    case INT_SYS_PAGE_ALLOC: {
        uintptr_t addr = current->p_registers.reg_eax;
        // Exercise 1: Your code here
        //   sys_page_alloc should not be able to map a page to virtual address
        //   under PROC_START_ADDR or to the page right before MEMSIZE_VIRTUAL
        //   (which would be used as the process's stack later)

        if(addr < PROC_START_ADDR || addr >= MEMSIZE_VIRTUAL - PAGESIZE)
        {
            current->p_registers.reg_eax = -1;
            break;
        }


        // Exercise 3: your code here
        uintptr_t freepage = find_free_physic_page(current->p_pid);
        virtual_memory_map(current->p_pagetable, addr, freepage,
                           PAGESIZE, PTE_P|PTE_W|PTE_U);
        current->p_registers.reg_eax = 1;
        break;
    }

    case INT_SYS_FORK: {
        current->p_registers.reg_eax = fork();
        break;
    }

    case INT_TIMER:
        ++ticks;
        schedule();
        break;                  /* will not be reached */

    case INT_PAGEFAULT: {
        // Analyze faulting address and access type.
        uintptr_t addr = rcr2();
        const char* operation = reg->reg_err & PFERR_WRITE
                ? "write" : "read";
        const char* problem = reg->reg_err & PFERR_PRESENT
                ? "protection problem" : "missing page";

        if (!(reg->reg_err & PFERR_USER))
            panic("Kernel page fault for %p (%s %s, eip=%p)!\n",
                  addr, operation, problem, reg->reg_eip);
        console_printf(CPOS(24, 0), 0x0C00,
                       "Process %d page fault for %p (%s %s, eip=%p)!\n",
                       current->p_pid, addr, operation, problem, reg->reg_eip);
        current->p_state = P_BROKEN;
        break;
    }

    default:
        panic("Unexpected exception %d!\n", reg->reg_intno);
        break;                  /* will not be reached */

    }


    // Return to the current process (or run something else).
    if (current->p_state == P_RUNNABLE)
        run(current);
    else
        schedule();
}


int fork(void) {
    pid_t pid;
    for (pid = 1; pid < NPROC && processes[pid].p_state != P_FREE; ++pid)
        ;
    if (pid >= NPROC)
        return -1;

    // Exercise 5: your code here

    processes[pid].p_registers = current->p_registers;
    processes[pid].p_registers.reg_eax = 0;
    processes[pid].p_state = P_RUNNABLE;
    return pid;
}


// schedule
//    Pick the next process to run and then run it.
//    If there are no runnable processes, spins forever.

void schedule(void) {
    pid_t pid = current->p_pid;
    while (1) {
        pid = (pid + 1) % NPROC;
        if (processes[pid].p_state == P_RUNNABLE)
            run(&processes[pid]);
        // If Control-C was typed, exit the virtual machine.
        check_keyboard();
    }
}


// run(p)
//    Run process `p`. This means reloading all the registers from
//    `p->p_registers` using the `popal`, `popl`, and `iret` instructions.
//
//    As a side effect, sets `current = p`.

void run(proc* p) {
    assert(p->p_state == P_RUNNABLE);
    current = p;

    set_pagetable(p->p_pagetable);
    asm volatile("movl %0,%%esp\n\t"
                 "popal\n\t"
                 "popl %%es\n\t"
                 "popl %%ds\n\t"
                 "addl $8, %%esp\n\t"
                 "iret"
                 :
                 : "g" (&p->p_registers)
                 : "memory");

 spinloop: goto spinloop;       // should never get here
}


// pageinfo_init
//    Initialize the `pageinfo[]` array.

void pageinfo_init(void) {
    extern char end[];

    for (uintptr_t addr = 0; addr < MEMSIZE_PHYSICAL; addr += PAGESIZE) {
        int owner;
        if (physical_memory_isreserved(addr))
            owner = PO_RESERVED;
        else if ((addr >= KERNEL_START_ADDR && addr < (uintptr_t) end)
                 || addr == KERNEL_STACK_TOP - PAGESIZE)
            owner = PO_KERNEL;
        else
            owner = PO_FREE;
        pageinfo[PAGENUMBER(addr)].owner = owner;
        pageinfo[PAGENUMBER(addr)].refcount = (owner != PO_FREE);
    }
}


// virtual_memory_check
//    Check operating system invariants about virtual memory. Panic if any
//    of the invariants are false.

void virtual_memory_check(void) {
    // Process 0 must never be used.
    assert(processes[0].p_state == P_FREE);

    // The kernel page table should be owned by the kernel;
    // its reference count should equal 1, plus the number of processes
    // that don't have their own page tables.
    // Active processes have their own page tables. A process page table
    // should be owned by that process and have reference count 1.
    // All level-2 page tables must have reference count 1.

    // Calculate expected kernel refcount
    int expected_kernel_refcount = 1;
    for (int pid = 0; pid < NPROC; ++pid)
        if (processes[pid].p_state != P_FREE
            && processes[pid].p_pagetable == kernel_pagetable)
            ++expected_kernel_refcount;

    for (int pid = -1; pid < NPROC; ++pid) {
        if (pid >= 0 && processes[pid].p_state == P_FREE)
            continue;

        x86_pagetable* pagetable;
        int expected_owner, expected_refcount;
        if (pid < 0 || processes[pid].p_pagetable == kernel_pagetable) {
            pagetable = kernel_pagetable;
            expected_owner = PO_KERNEL;
            expected_refcount = expected_kernel_refcount;
        } else {
            pagetable = processes[pid].p_pagetable;
            expected_owner = pid;
            expected_refcount = 1;
        }

        // Check main (level-1) page table
        assert(PTE_ADDR(pagetable) == (uintptr_t) pagetable);
        assert(PAGENUMBER(pagetable) < NPAGES);
        assert(pageinfo[PAGENUMBER(pagetable)].owner == expected_owner);
        assert(pageinfo[PAGENUMBER(pagetable)].refcount == expected_refcount);

        // Check level-2 page tables
        for (int pn = 0; pn < PAGETABLE_NENTRIES; ++pn)
            if (pagetable->entry[pn] & PTE_P) {
                x86_pageentry_t pte = pagetable->entry[pn];
                assert(PAGENUMBER(pte) < NPAGES);
                assert(pageinfo[PAGENUMBER(pte)].owner == expected_owner);
                assert(pageinfo[PAGENUMBER(pte)].refcount == 1);
            }
    }

    // Check that all referenced pages refer to active processes
    for (int pn = 0; pn < PAGENUMBER(MEMSIZE_PHYSICAL); ++pn)
        if (pageinfo[pn].refcount > 0 && pageinfo[pn].owner >= 0)
            assert(processes[pageinfo[pn].owner].p_state != P_FREE);
}


// memshow_physical
//    Draw a picture of physical memory on the CGA console.

static const uint16_t memstate_colors[] = {
    'K' | 0x0D00, 'R' | 0x0700, '.' | 0x0700, '1' | 0x0C00,
    '2' | 0x0A00, '3' | 0x0900, '4' | 0x0E00, '5' | 0x0F00,
    '6' | 0x0C00, '7' | 0x0A00, '8' | 0x0900, '9' | 0x0E00,
    'A' | 0x0F00, 'B' | 0x0C00, 'C' | 0x0A00, 'D' | 0x0900,
    'E' | 0x0E00, 'F' | 0x0F00
};

void memshow_physical(void) {
    console_printf(CPOS(0, 32), 0x0F00, "PHYSICAL MEMORY");
    for (int pn = 0; pn < PAGENUMBER(MEMSIZE_PHYSICAL); ++pn) {
        if (pn % 64 == 0)
            console_printf(CPOS(1 + pn / 64, 3), 0x0F00, "0x%06X ", pn << 12);

        int owner = pageinfo[pn].owner;
        if (pageinfo[pn].refcount == 0)
            owner = PO_FREE;
        uint16_t color = memstate_colors[owner - PO_KERNEL];
        // darker color for shared pages
        if (pageinfo[pn].refcount > 1)
            color &= 0x77FF;

        console[CPOS(1 + pn / 64, 12 + pn % 64)] = color;
    }
}


// memshow_virtual(pagetable, name)
//    Draw a picture of the virtual memory map `pagetable` (named `name`) on
//    the CGA console.

void memshow_virtual(x86_pagetable* pagetable, const char* name) {
    assert((uintptr_t) pagetable == PTE_ADDR(pagetable));

    console_printf(CPOS(10, 26), 0x0F00, "VIRTUAL ADDRESS SPACE FOR %s", name);
    for (uintptr_t va = 0; va < MEMSIZE_VIRTUAL; va += PAGESIZE) {
        vamapping vam = virtual_memory_lookup(pagetable, va);
        uint16_t color;
        if (vam.pn < 0)
            color = ' ';
        else {
            assert(vam.pa < MEMSIZE_PHYSICAL);
            int owner = pageinfo[vam.pn].owner;
            if (pageinfo[vam.pn].refcount == 0)
                owner = PO_FREE;
            color = memstate_colors[owner - PO_KERNEL];
            // reverse video for user-accessible pages
            if (vam.perm & PTE_U)
                color = ((color & 0x0F00) << 4) | ((color & 0xF000) >> 4)
                    | (color & 0x00FF);
            // darker color for shared pages
            if (pageinfo[vam.pn].refcount > 1)
                color &= 0x77FF;
        }
        uint32_t pn = PAGENUMBER(va);
        if (pn % 64 == 0)
            console_printf(CPOS(11 + pn / 64, 3), 0x0F00, "0x%06X ", va);
        console[CPOS(11 + pn / 64, 12 + pn % 64)] = color;
    }
}


// memshow_virtual_animate
//    Draw a picture of process virtual memory maps on the CGA console.
//    Starts with process 1, then switches to a new process every 0.25 sec.

void memshow_virtual_animate(void) {
    static unsigned last_ticks = 0;
    static int showing = 1;
#if TICKTYPE == 1
    static unsigned subticks = 0;
#endif

    // switch to a new process every 0.25 sec
#if TICKTYPE == 1
    if (last_ticks > ticks)
        last_ticks = ticks, showing = 1, subticks = 0;
    else
#endif
    if (last_ticks == 0 || ticks - last_ticks >= HZ / 2) {
        last_ticks = ticks;
        ++showing;
    }
#if TICKTYPE == 1
    else if (++subticks == 100)
        ++ticks, subticks = 0;
#endif

    // the current process may have died -- don't display it if so
    while (showing <= 2*NPROC && processes[showing % NPROC].p_state == P_FREE)
        ++showing;
    showing = showing % NPROC;

    if (processes[showing].p_state != P_FREE) {
        char s[4];
        snprintf(s, 4, "%d ", showing);
        memshow_virtual(processes[showing].p_pagetable, s);
    }
}
