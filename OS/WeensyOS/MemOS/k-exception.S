###############################################################################
# Exception handlers
#
#   Assembly code defining kernel exception handlers
#   (for interrupts, traps, and faults).

.text

# First, some magic so the OS follows the "Multiboot" standard.
# It must come first in the binary.

.globl multiboot
multiboot:
        .long 0x1BADB002
        .long 0
        .long -0x1BADB002

# The multiboot_start routine sets the stack pointer to the top of the
# OS kernel stack, then jumps to the `kernel` routine.

.globl multiboot_start
multiboot_start:
        movl $0x80000, %esp
        pushl $0
        popfl
        // Check for multiboot command line; if found pass it along.
        cmpl $0x2BADB002, %eax
        jne 1f
        testl $4, (%ebx)
        je 1f
        movl 16(%ebx), %ebx
        jmp 2f
1:      movl $0x0, %ebx
2:      pushl %ebx
        call kernel


# Interrupt handlers
.align 2

        .globl gpf_int_handler
gpf_int_handler:
        pushl $13               // trap number
        jmp _generic_int_handler

        .globl pagefault_int_handler
pagefault_int_handler:
        pushl $14
        jmp _generic_int_handler

        .globl timer_int_handler
timer_int_handler:
        pushl $0                // error code
        pushl $32
        jmp _generic_int_handler

sys48_int_handler:
        pushl $0
        pushl $48
        jmp _generic_int_handler

sys49_int_handler:
        pushl $0
        pushl $49
        jmp _generic_int_handler

sys50_int_handler:
        pushl $0
        pushl $50
        jmp _generic_int_handler

sys51_int_handler:
        pushl $0
        pushl $51
        jmp _generic_int_handler

sys52_int_handler:
        pushl $0
        pushl $52
        jmp _generic_int_handler

sys53_int_handler:
        pushl $0
        pushl $53
        jmp _generic_int_handler

sys54_int_handler:
        pushl $0
        pushl $54
        jmp _generic_int_handler

sys55_int_handler:
        pushl $0
        pushl $55
        jmp _generic_int_handler

sys56_int_handler:
        pushl $0
        pushl $56
        jmp _generic_int_handler

sys57_int_handler:
        pushl $0
        pushl $57
        jmp _generic_int_handler

sys58_int_handler:
        pushl $0
        pushl $58
        jmp _generic_int_handler

sys59_int_handler:
        pushl $0
        pushl $59
        jmp _generic_int_handler

sys60_int_handler:
        pushl $0
        pushl $60
        jmp _generic_int_handler

sys61_int_handler:
        pushl $0
        pushl $61
        jmp _generic_int_handler

sys62_int_handler:
        pushl $0
        pushl $62
        jmp _generic_int_handler

sys63_int_handler:
        pushl $0
        pushl $63
        jmp _generic_int_handler

        .globl default_int_handler
default_int_handler:
        pushl $0
        jmp _generic_int_handler


_generic_int_handler:
        # When we get here, the processor's exception mechanism has
        # pushed the old task status and stack registers onto the kernel stack.
        # Then one of the specific handlers pushed the exception number.
        # Now, we complete the `struct x86_registers` by pushing the extra
        # segment definitions and the general CPU registers.
        pushl %ds
        pushl %es
        pushal

        # Load the data segments with the kernel's values.
        movl $0x10, %eax                # SEE ALSO k-hardware.c
        movw %ax, %ds
        movw %ax, %es

        # Call the kernel's `exception` function.
        pushl %esp                      # 1st argument points at `registers`
        call exception
        # `exception` should never return.


        # An array of function pointers to the interrupt handlers.
        .globl sys_int_handlers
sys_int_handlers:
        .long sys48_int_handler
        .long sys49_int_handler
        .long sys50_int_handler
        .long sys51_int_handler
        .long sys52_int_handler
        .long sys53_int_handler
        .long sys54_int_handler
        .long sys55_int_handler
        .long sys56_int_handler
        .long sys57_int_handler
        .long sys58_int_handler
        .long sys59_int_handler
        .long sys60_int_handler
        .long sys61_int_handler
        .long sys62_int_handler
        .long sys63_int_handler


.section .note.GNU-stack,"",@progbits
