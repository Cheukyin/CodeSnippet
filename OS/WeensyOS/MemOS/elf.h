#ifndef WEENSYOS_ELF_H
#define WEENSYOS_ELF_H

// elf.h
//
//   Structures and constants for ELF (Executable Linking Format) executable
//   files.

#define ELF_MAGIC 0x464C457FU   // "\x7FELF" in little endian

// executable header
typedef struct elf_header {
    uint32_t e_magic;           // must equal ELF_MAGIC
    uint8_t e_elf[12];
    uint16_t e_type;
    uint16_t e_machine;
    uint32_t e_version;
    uintptr_t e_entry;          // entry point: address of first instruction
    uint32_t e_phoff;           // offset from elf_header to 1st elf_program
    uint32_t e_shoff;           // offset from elf_header to 1st elf_section
    uint32_t e_flags;
    uint16_t e_ehsize;
    uint16_t e_phentsize;       // should equal sizeof(elf_program)
    uint16_t e_phnum;           // number of elf_programs
    uint16_t e_shentsize;       // should equal sizeof(elf_section)
    uint16_t e_shnum;           // number of elf_sections
    uint16_t e_shstrndx;
} elf_header;

// program header (required by the loader)
typedef struct elf_program {
    uint32_t p_type;            // see ELF_PTYPE below
    uint32_t p_offset;          // offset from elf_header to program data
    void* p_va;                 // virtual address to load data
    void* p_pa;                 // not used
    uint32_t p_filesz;          // number of bytes of program data
    uint32_t p_memsz;           // number of bytes in memory (any bytes beyond
                                //   p_filesz are initialized to zero)
    uint32_t p_flags;           // see ELF_PFLAG below
    uint32_t p_align;
} elf_program;

typedef struct elf_section {
    uint32_t s_name;
    uint32_t s_type;
    uint32_t s_flags;
    uint32_t s_addr;
    uint32_t s_offset;
    uint32_t s_size;
    uint32_t s_link;
    uint32_t s_info;
    uint32_t s_addralign;
    uint32_t s_entsize;
} elf_section;

// Values for elf_prog::p_type
#define ELF_PTYPE_LOAD          1

// Flag bits for elf_prog::p_flags
#define ELF_PFLAG_EXEC          1
#define ELF_PFLAG_WRITE         2
#define ELF_PFLAG_READ          4

// Values for elf_section::s_type
#define ELF_STYPE_NULL          0
#define ELF_STYPE_PROGBITS      1
#define ELF_STYPE_SYMTAB        2
#define ELF_STYPE_STRTAB        3

// Values for elf_section::s_name
#define ELF_SNAME_UNDEF         0

#endif /* !JOS_INC_ELF_H */
