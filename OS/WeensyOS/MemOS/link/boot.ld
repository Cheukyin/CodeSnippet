OUTPUT_FORMAT("elf32-i386", "elf32-i386", "elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(boot_start)

SECTIONS {
    . = 0x7c00;

    /* Text segment: instructions only.
       The boot loader must fit in 512 bytes; there are no globals. */
    .text : { *(.text .stub .text.* .gnu.linkonce.t.*) }

    /DISCARD/ : { *(.eh_frame .note.GNU-stack) }
}
