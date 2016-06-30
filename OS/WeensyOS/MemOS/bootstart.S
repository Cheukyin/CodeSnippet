###############################################################################
# BOOT ENTRY POINT
#
#   After the BIOS initializes the hardware on startup or system reset,
#   it loads the first 512-byte sector of the hard disk
#   into physical addresses 0x7C00-0x7DFF.
#   Then it jumps to address 0x7C00 -- and the OS starts running!
#
#   This file contains the code loaded at that address.
#   The `boot_start` routine switches the CPU out of compatibility mode,
#   which requires some subtle assembly programming.
#   Then it calls the `boot` routine to finish the booting
#   process.  This routine is defined in `boot.c`.
#
#   There is no need to understand this code in detail!
#   (You may stop reading now.)
#
###############################################################################


###############################################################################
# For Your Information: COMPATIBILITY MODES
#
#   The Intel x86 architecture has many compatibility modes, going back to
#   the 8086, which supported only 16-bit addresses.  When the BIOS calls
#   into the OS, it is running in the "most compatible" mode, 16-bit real
#   mode.  The machine acts like addresses are only 16 bits long,
#   there's no paging support, and there isn't even any support for
#   user-mode applications -- the processor is always privileged, so there's
#   no way to implement memory protection!  We want to get into "386" mode,
#   with 32-bit addresses and memory protection, as soon as possible.  And
#   that's what this code does.
#
#   Again, there is no need to understand this code in detail!
#
###############################################################################

# Define some constants
.set SEGSEL_BOOT_CODE,0x8       # code segment selector
.set SEGSEL_BOOT_DATA,0x10      # data segment selector
.set CR0_PE_ON,0x1              # protected mode enable flag

.globl boot_start                               # Entry point
boot_start:     .code16                         # This runs in real mode
                cli                             # Disable interrupts
                cld                             # String operations increment

                # Set up the important data segment registers (DS, ES, SS).
                xorw    %ax,%ax                 # Segment number zero
                movw    %ax,%ds                 # -> Data Segment
                movw    %ax,%es                 # -> Extra Segment
                movw    %ax,%ss                 # -> Stack Segment

                # Set up the stack pointer, growing downward from 0x7c00.
                movw    $boot_start,%sp         # Stack Pointer

# Enable A20:
#   For fascinating historical reasons (related to the fact that
#   the earliest 8086-based PCs could only address 1MB of physical memory
#   and subsequent 80286-based PCs wanted to retain maximum compatibility),
#   physical address line 20 is tied to low when the machine boots.
#   Obviously this a bit of a drag for us, especially when trying to
#   address memory above 1MB.  This code undoes this.

seta20.1:       inb     $0x64,%al               # Get status
                testb   $0x2,%al                # Busy?
                jnz     seta20.1                # Yes
                movb    $0xd1,%al               # Command: Write
                outb    %al,$0x64               #  output port
seta20.2:       inb     $0x64,%al               # Get status
                testb   $0x2,%al                # Busy?
                jnz     seta20.2                # Yes
                movb    $0xdf,%al               # Enable
                outb    %al,$0x60               #  A20

# Switch from real to protected mode:
#   Up until now, there's been no protection, so we've gotten along perfectly
#   well without explicitly telling the processor how to translate addresses.
#   When we switch to protected mode, this is no longer true!
#   We need at least to set up some "segments" that tell the processor it's
#   OK to run code at any address, or write to any address.
#   The 'gdt' and 'gdtdesc' tables below define these segments.
#   This code loads them into the processor.
#   We need this setup to ensure the transition to protected mode is smooth.

real_to_prot:   cli                     # Don't allow interrupts: mandatory,
                                        # since we didn't set up an interrupt
                                        # descriptor table for handling them
                lgdt    gdtdesc         # load GDT: mandatory in protected mode
                movl    %cr0, %eax      # Turn on protected mode
                orl     $CR0_PE_ON, %eax
                movl    %eax, %cr0

                # CPU magic: jump to relocation, flush prefetch queue, and
                # reload %cs.  Has the effect of just jmp to the next
                # instruction, but simultaneously loads CS with
                # $SEGSEL_BOOT_CODE.
                ljmp    $SEGSEL_BOOT_CODE, $protcseg

                .code32                 # run in 32-bit protected mode
                # Set up the protected-mode data segment registers
protcseg:       movw    $SEGSEL_BOOT_DATA, %ax  # Our data segment selector
                movw    %ax, %ds                # -> DS: Data Segment
                movw    %ax, %es                # -> ES: Extra Segment
                movw    %ax, %fs                # -> FS
                movw    %ax, %gs                # -> GS
                movw    %ax, %ss                # -> SS: Stack Segment

                call boot               # finish the boot!  Shouldn't return,

spinloop:       jmp spinloop            # ..but in case it does, spin.


# Segment descriptors

# These macros are used to define segment descriptors.
#define SEG_NULL                                                        \
                .word 0, 0;                                             \
                .byte 0, 0, 0, 0
#define SEG(type,base,lim)                                              \
                .word (((lim) >> 12) & 0xffff), ((base) & 0xffff);      \
                .byte (((base) >> 16) & 0xff), (0x90 | (type)),         \
                (0xC0 | (((lim) >> 28) & 0xf)), (((base) >> 24) & 0xff)
#define STA_X   0x8         // Executable segment
#define STA_W   0x2         // Writeable (non-executable segments)
#define STA_R   0x2         // Readable (executable segments)

                .p2align 2                      # force 4 byte alignment
gdt:            SEG_NULL                                # null seg
                SEG(STA_X|STA_R, 0x0, 0xffffffff)       # code seg
                SEG(STA_W, 0x0, 0xffffffff)             # data seg

gdtdesc:        .word   0x17                    # sizeof(gdt) - 1
                .long   gdt                     # address gdt
