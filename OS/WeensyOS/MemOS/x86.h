#ifndef WEENSYOS_X86_H
#define WEENSYOS_X86_H
#include "lib.h"

// x86.h: C code to interface with x86 hardware and CPU.
//
//   Contents:
//   - Memory and interrupt constants.
//   - x86_registers: Used in process descriptors to store x86 registers.
//   - x86 functions: C function wrappers for useful x86 instructions.
//   - Hardware structures: C structures and constants for initializing
//     x86 hardware, including the interrupt descriptor table.

// Paged memory constants
#define PAGESHIFT       12                // log_2 (size of page in bytes)
#define PAGESIZE        (1 << PAGESHIFT)  // Size of page in bytes
#define PAGETABLE_NENTRIES (1 << 10)      // # entries per page table page
#define PAGENUMBER(ptr) ((uintptr_t) (ptr) >> PAGESHIFT)
#define PAGEADDRESS(pn) ((uintptr_t) (pn) << PAGESHIFT)

// Page table entry type and page table type
typedef uint32_t x86_pageentry_t;
typedef struct __attribute__((aligned(PAGESIZE))) x86_pagetable {
    x86_pageentry_t entry[PAGETABLE_NENTRIES];
} x86_pagetable;

// Parts of a paged address: level-1 page table index,
// level-2 page table index, and page offset
#define L1PAGEINDEX(addr) ((uintptr_t) (addr) >> 22)
#define L2PAGEINDEX(addr) (((uintptr_t) (addr) >> 12) & 0x3FFU)
#define PAGEOFFMASK       ((uintptr_t) (PAGESIZE - 1))
#define PAGEOFFSET(addr)  ((uintptr_t) (addr) & PAGEOFFMASK)

// The physical address contained in a page table entry
#define PTE_ADDR(pageentry)     ((uintptr_t) (pageentry) & ~0xFFFU)

// Page table entry flags
#define PTE_FLAGS(pageentry)    ((x86_pageentry_t) (pageentry) & 0xFFFU)
// - Permission flags: define whether page is accessible
#define PTE_P        ((x86_pageentry_t) 1)  // entry is Present
#define PTE_W        ((x86_pageentry_t) 2)  // entry is Writeable
#define PTE_U        ((x86_pageentry_t) 4)  // entry is User-accessible
// - Accessed flags: automatically turned on by processor
#define PTE_A        ((x86_pageentry_t) 32) // entry was Accessed (read/written)
#define PTE_D        ((x86_pageentry_t) 64) // entry was Dirtied (written)
// - There are other flags too!

// Page fault error flags
// These bits are stored in x86_registers::reg_err after a page fault trap.
#define PFERR_PRESENT   0x1             // Fault happened due to a protection
                                        //   violation (rather than due to a
                                        //   missing page)
#define PFERR_WRITE     0x2             // Fault happened on a write
#define PFERR_USER      0x4             // Fault happened in an application
                                        //   (user mode) (rather than kernel)


// Interrupt numbers
#define INT_DIVIDE      0x0         // Divide error
#define INT_DEBUG       0x1         // Debug exception
#define INT_BREAKPOINT  0x3         // Breakpoint
#define INT_OVERFLOW    0x4         // Overflow
#define INT_BOUNDS      0x5         // Bounds check
#define INT_INVALIDOP   0x6         // Invalid opcode
#define INT_DOUBLEFAULT 0x8         // Double fault
#define INT_INVALIDTSS  0xA         // Invalid TSS
#define INT_SEGMENT     0xB         // Segment not present
#define INT_STACK       0xC         // Stack exception
#define INT_GPF         0xD         // General protection fault
#define INT_PAGEFAULT   0xE         // Page fault


// struct x86_registers
//     A complete set of x86 general-purpose registers, plus some
//     special-purpose registers. The order and contents are defined to make
//     it more convenient to use important x86 instructions.

typedef struct x86_registers {
    uint32_t reg_edi;           // (1) General CPU registers
    uint32_t reg_esi;           // order defined by 'pushal'
    uint32_t reg_ebp;
    uint32_t reg_original_esp;
    uint32_t reg_ebx;
    uint32_t reg_edx;
    uint32_t reg_ecx;
    uint32_t reg_eax;

    uint16_t reg_es;            // (2) Extra segments %es and %ds
    uint16_t reg_padding1;
    uint16_t reg_ds;
    uint16_t reg_padding2;

    uint32_t reg_intno;         // (3) Interrupt number and error
    uint32_t reg_err;           // code (optional; supplied by x86
                                // interrupt mechanism)

    uint32_t reg_eip;           // (4) Task status
    uint16_t reg_cs;            // EIP, code segment, flags (eflags)
    uint16_t reg_padding3;      // in the order required by 'iret'
    uint32_t reg_eflags;

    uint32_t reg_esp;           // (5) Stack registers
    uint16_t reg_ss;            // in the order required by 'iret'
    uint16_t reg_padding4;      // when changing privilege (e.g.
                                // returning from kernel to user)
} x86_registers;


// x86 functions: Inline C functions that execute useful x86 instructions.
//
//      Also some macros corresponding to x86 register flag bits.

#define DECLARE_X86_FUNCTION(function_prototype) \
        static inline function_prototype __attribute__((always_inline))

DECLARE_X86_FUNCTION(void       breakpoint(void));
DECLARE_X86_FUNCTION(uint8_t    inb(int port));
DECLARE_X86_FUNCTION(void       insb(int port, void* addr, int cnt));
DECLARE_X86_FUNCTION(uint16_t   inw(int port));
DECLARE_X86_FUNCTION(void       insw(int port, void* addr, int cnt));
DECLARE_X86_FUNCTION(uint32_t   inl(int port));
DECLARE_X86_FUNCTION(void       insl(int port, void* addr, int cnt));
DECLARE_X86_FUNCTION(void       outb(int port, uint8_t data));
DECLARE_X86_FUNCTION(void       outsb(int port, const void* addr, int cnt));
DECLARE_X86_FUNCTION(void       outw(int port, uint16_t data));
DECLARE_X86_FUNCTION(void       outsw(int port, const void* addr, int cnt));
DECLARE_X86_FUNCTION(void       outsl(int port, const void* addr, int cnt));
DECLARE_X86_FUNCTION(void       outl(int port, uint32_t data));
DECLARE_X86_FUNCTION(void       invlpg(void* addr));
DECLARE_X86_FUNCTION(void       lidt(void* p));
DECLARE_X86_FUNCTION(void       lldt(uint16_t sel));
DECLARE_X86_FUNCTION(void       ltr(uint16_t sel));
DECLARE_X86_FUNCTION(void       lcr0(uint32_t val));
DECLARE_X86_FUNCTION(uint32_t   rcr0(void));
DECLARE_X86_FUNCTION(uintptr_t  rcr2(void));
DECLARE_X86_FUNCTION(void       lcr3(uintptr_t val));
DECLARE_X86_FUNCTION(uintptr_t  rcr3(void));
DECLARE_X86_FUNCTION(void       lcr4(uint32_t val));
DECLARE_X86_FUNCTION(uint32_t   rcr4(void));
DECLARE_X86_FUNCTION(void       cli(void));
DECLARE_X86_FUNCTION(void       sti(void));
DECLARE_X86_FUNCTION(void       tlbflush(void));
DECLARE_X86_FUNCTION(uint32_t   read_eflags(void));
DECLARE_X86_FUNCTION(void       write_eflags(uint32_t eflags));
DECLARE_X86_FUNCTION(uint32_t   read_ebp(void));
DECLARE_X86_FUNCTION(uint32_t   read_esp(void));
DECLARE_X86_FUNCTION(void       cpuid(uint32_t info, uint32_t* eaxp,
                                      uint32_t* ebxp, uint32_t* ecxp,
                                      uint32_t* edxp));
DECLARE_X86_FUNCTION(uint64_t   read_cycle_counter(void));

// %cr0 flag bits (useful for lcr0() and rcr0())
#define CR0_PE                  0x00000001      // Protection Enable
#define CR0_MP                  0x00000002      // Monitor coProcessor
#define CR0_EM                  0x00000004      // Emulation
#define CR0_TS                  0x00000008      // Task Switched
#define CR0_ET                  0x00000010      // Extension Type
#define CR0_NE                  0x00000020      // Numeric Errror
#define CR0_WP                  0x00010000      // Write Protect
#define CR0_AM                  0x00040000      // Alignment Mask
#define CR0_NW                  0x20000000      // Not Writethrough
#define CR0_CD                  0x40000000      // Cache Disable
#define CR0_PG                  0x80000000      // Paging

// eflags bits (useful for read_eflags() and write_eflags())
#define EFLAGS_CF               0x00000001      // Carry Flag
#define EFLAGS_PF               0x00000004      // Parity Flag
#define EFLAGS_AF               0x00000010      // Auxiliary carry Flag
#define EFLAGS_ZF               0x00000040      // Zero Flag
#define EFLAGS_SF               0x00000080      // Sign Flag
#define EFLAGS_TF               0x00000100      // Trap Flag
#define EFLAGS_IF               0x00000200      // Interrupt Flag
#define EFLAGS_DF               0x00000400      // Direction Flag
#define EFLAGS_OF               0x00000800      // Overflow Flag
#define EFLAGS_IOPL_MASK        0x00003000      // I/O Privilege Level bitmask
#define EFLAGS_IOPL_0           0x00000000      //   IOPL == 0
#define EFLAGS_IOPL_1           0x00001000      //   IOPL == 1
#define EFLAGS_IOPL_2           0x00002000      //   IOPL == 2
#define EFLAGS_IOPL_3           0x00003000      //   IOPL == 3
#define EFLAGS_NT               0x00004000      // Nested Task
#define EFLAGS_RF               0x00010000      // Resume Flag
#define EFLAGS_VM               0x00020000      // Virtual 8086 mode
#define EFLAGS_AC               0x00040000      // Alignment Check
#define EFLAGS_VIF              0x00080000      // Virtual Interrupt Flag
#define EFLAGS_VIP              0x00100000      // Virtual Interrupt Pending
#define EFLAGS_ID               0x00200000      // ID flag

static inline void breakpoint(void) {
    asm volatile("int3");
}

static inline uint8_t inb(int port) {
    uint8_t data;
    asm volatile("inb %w1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline void insb(int port, void* addr, int cnt) {
    asm volatile("cld\n\trepne\n\tinsb"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
}

static inline uint16_t inw(int port) {
    uint16_t data;
    asm volatile("inw %w1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline void insw(int port, void* addr, int cnt) {
    asm volatile("cld\n\trepne\n\tinsw"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
}

static inline uint32_t inl(int port) {
    uint32_t data;
    asm volatile("inl %w1,%0" : "=a" (data) : "d" (port));
    return data;
}

static inline void insl(int port, void* addr, int cnt) {
    asm volatile("cld\n\trepne\n\tinsl"
                 : "=D" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "memory", "cc");
}

static inline void outb(int port, uint8_t data) {
    asm volatile("outb %0,%w1" : : "a" (data), "d" (port));
}

static inline void outsb(int port, const void* addr, int cnt) {
    asm volatile("cld\n\trepne\n\toutsb"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
}

static inline void outw(int port, uint16_t data) {
    asm volatile("outw %0,%w1" : : "a" (data), "d" (port));
}

static inline void outsw(int port, const void* addr, int cnt) {
    asm volatile("cld\n\trepne\n\toutsw"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
}

static inline void outsl(int port, const void* addr, int cnt) {
    asm volatile("cld\n\trepne\n\toutsl"
                 : "=S" (addr), "=c" (cnt)
                 : "d" (port), "0" (addr), "1" (cnt)
                 : "cc");
}

static inline void outl(int port, uint32_t data) {
    asm volatile("outl %0,%w1" : : "a" (data), "d" (port));
}

static inline void invlpg(void* addr) {
    asm volatile("invlpg (%0)" : : "r" (addr) : "memory");
}

static inline void lidt(void* p) {
    asm volatile("lidt (%0)" : : "r" (p));
}

static inline void lldt(uint16_t sel) {
    asm volatile("lldt %0" : : "r" (sel));
}

static inline void ltr(uint16_t sel) {
    asm volatile("ltr %0" : : "r" (sel));
}

static inline void lcr0(uint32_t val) {
    asm volatile("movl %0,%%cr0" : : "r" (val));
}

static inline uint32_t rcr0(void) {
    uint32_t val;
    asm volatile("movl %%cr0,%0" : "=r" (val));
    return val;
}

static inline uintptr_t rcr2(void) {
    uintptr_t val;
    asm volatile("movl %%cr2,%0" : "=r" (val));
    return val;
}

static inline void lcr3(uintptr_t val) {
    asm volatile("movl %0,%%cr3" : : "r" (val));
}

static inline uintptr_t rcr3(void) {
    uintptr_t val;
    asm volatile("movl %%cr3,%0" : "=r" (val));
    return val;
}

static inline void lcr4(uint32_t val) {
    asm volatile("movl %0,%%cr4" : : "r" (val));
}

static inline uint32_t rcr4(void) {
    uint32_t cr4;
    asm volatile("movl %%cr4,%0" : "=r" (cr4));
    return cr4;
}

static inline void cli(void) {
    asm volatile("cli");
}

static inline void sti(void) {
    asm volatile("sti");
}

static inline void tlbflush(void) {
    uint32_t cr3;
    asm volatile("movl %%cr3,%0" : "=r" (cr3));
    asm volatile("movl %0,%%cr3" : : "r" (cr3));
}

static inline uint32_t read_eflags(void) {
    uint32_t eflags;
    asm volatile("pushfl; popl %0" : "=r" (eflags));
    return eflags;
}

static inline void write_eflags(uint32_t eflags) {
    asm volatile("pushl %0; popfl" : : "r" (eflags));
}

static inline uint32_t read_ebp(void) {
    uint32_t ebp;
    asm volatile("movl %%ebp,%0" : "=r" (ebp));
    return ebp;
}

static inline uint32_t read_esp(void) {
    uint32_t esp;
    asm volatile("movl %%esp,%0" : "=r" (esp));
    return esp;
}

static inline void cpuid(uint32_t info, uint32_t* eaxp, uint32_t* ebxp,
                         uint32_t* ecxp, uint32_t* edxp) {
    uint32_t eax, ebx, ecx, edx;
    asm volatile("cpuid"
                 : "=a" (eax), "=b" (ebx), "=c" (ecx), "=d" (edx)
                 : "a" (info));
    if (eaxp)
        *eaxp = eax;
    if (ebxp)
        *ebxp = ebx;
    if (ecxp)
        *ecxp = ecx;
    if (edxp)
        *edxp = edx;
}

static inline uint64_t read_cycle_counter(void) {
    uint64_t tsc;
    asm volatile("rdtsc" : "=A" (tsc));
    return tsc;
}

static inline uint32_t fetch_and_addl(uint32_t* object, uint32_t addend) {
    asm volatile("lock; xaddl %0, %1"
                 : "+r" (addend), "+m" (*object)
                 :
                 : "cc");
    return addend;
}

static inline void fence() {
    asm volatile("" : : : "memory");
}


// Hardware definitions: C structures and constants for initializing x86
// hardware, particularly gate descriptors (loaded into the interrupt
// descriptor table) and segment descriptors.

// Gate descriptors for exceptions (interrupts, traps, and faults)
typedef struct x86_gatedescriptor {
        unsigned gd_off_15_0 : 16;   // low 16 bits of offset in segment
        unsigned gd_ss : 16;         // segment selector
        unsigned gd_args : 5;        // # args, 0 for interrupt/trap gates
        unsigned gd_rsv1 : 3;        // reserved(should be zero I guess)
        unsigned gd_type : 4;        // type(STS_{TG,IG32,TG32})
        unsigned gd_s : 1;           // must be 0 (system)
        unsigned gd_dpl : 2;         // descriptor(meaning new) privilege level
        unsigned gd_p : 1;           // Present
        unsigned gd_off_31_16 : 16;  // high bits of offset in segment
} x86_gatedescriptor;

// Set up a normal gate descriptor.
// - istrap: 1 for a trap gate, 0 for an interrupt gate.
// - sel: Code segment selector for handler
// - off: Offset in code segment for handler
// - dpl: Descriptor Privilege Level -
//        the privilege level required for software to invoke this gate
//        explicitly (as a trap) using an int instruction.
#define SETGATE(gate, istrap, sel, off, dpl)                    \
do {                                                            \
        (gate).gd_off_15_0 = (uint32_t) (off) & 0xffff;         \
        (gate).gd_ss = (sel);                                   \
        (gate).gd_args = 0;                                     \
        (gate).gd_rsv1 = 0;                                     \
        (gate).gd_type = (istrap) ? STS_TG32 : STS_IG32;        \
        (gate).gd_s = 0;                                        \
        (gate).gd_dpl = (dpl);                                  \
        (gate).gd_p = 1;                                        \
        (gate).gd_off_31_16 = (uint32_t) (off) >> 16;           \
} while (0)

// Pseudo-descriptors used for LGDT, LLDT, and LIDT instructions
typedef struct __attribute__((packed)) x86_pseudodescriptor {
        uint16_t idtd_lim;              // Limit
        uint32_t idtd_base;             // Base address
} x86_pseudodescriptor;

// Task state segment format (as described by the Pentium architecture book)
typedef struct x86_taskstate {
        uint32_t ts_link;       // Old ts selector
        uintptr_t ts_esp0;      // Stack pointers and segment selectors
        uint16_t ts_ss0;        //   after an increase in privilege level
        uint16_t ts_padding1;
        uintptr_t ts_esp1;
        uint16_t ts_ss1;
        uint16_t ts_padding2;
        uintptr_t ts_esp2;
        uint16_t ts_ss2;
        uint16_t ts_padding3;
        uint32_t ts_cr3;        // Page directory base (PHYSICAL address)
        uintptr_t ts_eip;       // Saved state from last task switch
        uint32_t ts_eflags;
        uint32_t ts_eax;        // More saved state (registers)
        uint32_t ts_ecx;
        uint32_t ts_edx;
        uint32_t ts_ebx;
        uintptr_t ts_esp;
        uintptr_t ts_ebp;
        uint32_t ts_esi;
        uint32_t ts_edi;
        uint16_t ts_es;         // Even more saved state (segment selectors)
        uint16_t ts_padding4;
        uint16_t ts_cs;
        uint16_t ts_padding5;
        uint16_t ts_ss;
        uint16_t ts_padding6;
        uint16_t ts_ds;
        uint16_t ts_padding7;
        uint16_t ts_fs;
        uint16_t ts_padding8;
        uint16_t ts_gs;
        uint16_t ts_padding9;
        uint16_t ts_ldt;
        uint16_t ts_padding10;
        uint16_t ts_t;          // Trap on task switch
        uint16_t ts_iomb;       // I/O map base address
} x86_taskstate;

// Segment descriptors
typedef struct x86_segmentdescriptor {
        unsigned sd_lim_15_0 : 16;  // Low bits of segment limit
        unsigned sd_base_15_0 : 16; // Low bits of segment base address
        unsigned sd_base_23_16 : 8; // Middle bits of segment base address
        unsigned sd_type : 4;       // Segment type (see STS_ constants)
        unsigned sd_s : 1;          // 0 = system, 1 = application
        unsigned sd_dpl : 2;        // Descriptor Privilege Level
        unsigned sd_p : 1;          // Present
        unsigned sd_lim_19_16 : 4;  // High bits of segment limit
        unsigned sd_avl : 1;        // Unused (available for software use)
        unsigned sd_rsv1 : 1;       // Reserved
        unsigned sd_db : 1;         // 0 = 16-bit segment, 1 = 32-bit segment
        unsigned sd_g : 1;          // Granularity: limit scaled by 4K when set
        unsigned sd_base_31_24 : 8; // High bits of segment base address
} x86_segmentdescriptor;

// Null segment
#define SEG_NULL        \
        { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

// Segment that is loadable but faults when used
#define SEG_FAULT       \
        { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0 }

// Normal segment
#define SEG(type, base, lim, dpl)                                       \
        {                                                               \
            ((lim) >> 12) & 0xFFFF,                                     \
            (base) & 0xFFFF, ((uintptr_t) (base) >> 16) & 0xFF,         \
            type, 1, dpl, 1, ((lim) >> 28) & 0xF,                       \
            0, 0, 1, 1, ((base) >> 24) & 0xFF                           \
        }

// 16-bit segment
#define SEG16(type, base, lim, dpl)                                     \
        (x86_segmentdescriptor) {                                       \
                (lim) & 0xFFFF,                                         \
                (base) & 0xFFFF, ((base) >> 16) & 0xFF,                 \
                type, 1, dpl, 1, ((lim) >> 16) & 0xF,                   \
                0, 0, 1, 0, ((base) >> 24) & 0xFF                       \
        }

// Application segment type bits
#define STA_X           0x8         // Executable segment
#define STA_E           0x4         // Expand down (non-executable segments)
#define STA_C           0x4         // Conforming code segment (executable only)
#define STA_W           0x2         // Writeable (non-executable segments)
#define STA_R           0x2         // Readable (executable segments)
#define STA_A           0x1         // Accessed

// System segment type bits
#define STS_T16A        0x1         // Available 16-bit TSS
#define STS_LDT         0x2         // Local Descriptor Table
#define STS_T16B        0x3         // Busy 16-bit TSS
#define STS_CG16        0x4         // 16-bit Call Gate
#define STS_TG          0x5         // Task Gate / Coum Transmitions
#define STS_IG16        0x6         // 16-bit Interrupt Gate
#define STS_TG16        0x7         // 16-bit Trap Gate
#define STS_T32A        0x9         // Available 32-bit TSS
#define STS_T32B        0xB         // Busy 32-bit TSS
#define STS_CG32        0xC         // 32-bit Call Gate
#define STS_IG32        0xE         // 32-bit Interrupt Gate
#define STS_TG32        0xF         // 32-bit Trap Gate

// Keyboard programmed I/O
#define KEYBOARD_STATUSREG      0x64
#define KEYBOARD_STATUS_READY   0x01
#define KEYBOARD_DATAREG        0x60

#endif /* !WEENSYOS_X86_H */
