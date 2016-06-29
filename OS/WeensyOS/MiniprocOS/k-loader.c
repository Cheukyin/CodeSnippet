#include "x86.h"
#include "elf.h"
#include "lib.h"
#include "kernel.h"

/*****************************************************************************
 * k-loader.c
 *
 *   Load a miniprocos application in from a RAM image.
 *
 *   Don't worry about understanding this loader!
 *
 *****************************************************************************/

#define SECTORSIZE		512
#define PAGESIZE		4096

extern uint8_t _binary_obj_p_procos_app_start[];
extern uint8_t _binary_obj_p_procos_app_end[];
extern uint8_t _binary_obj_p_procos_app2_start[];
extern uint8_t _binary_obj_p_procos_app2_end[];
extern uint8_t _binary_obj_p_procos_app3_start[];
extern uint8_t _binary_obj_p_procos_app3_end[];

struct ramimage {
	void *begin;
	void *end;
} ramimages[] = {
	{ _binary_obj_p_procos_app_start, _binary_obj_p_procos_app_end },
	{ _binary_obj_p_procos_app2_start, _binary_obj_p_procos_app2_end },
	{ _binary_obj_p_procos_app3_start, _binary_obj_p_procos_app3_end }
};

static void copyseg(void *dst, const uint8_t *src,
		    uint32_t filesz, uint32_t memsz);
static void loader_panic(void);

void
program_loader(int program_id, uint32_t *entry_point)
{
	struct Proghdr *ph, *eph;
	struct Elf *elf_header;
	int nprograms = sizeof(ramimages) / sizeof(ramimages[0]);

	if (program_id < 0 || program_id >= nprograms)
		loader_panic();

	// is this a valid ELF?
	elf_header = (struct Elf *) ramimages[program_id].begin;
	if (elf_header->e_magic != ELF_MAGIC)
		loader_panic();

	// load each program segment (ignores ph flags)
	ph = (struct Proghdr*) ((const uint8_t *) elf_header + elf_header->e_phoff);
	eph = ph + elf_header->e_phnum;
	for (; ph < eph; ph++)
		if (ph->p_type == ELF_PROG_LOAD)
			copyseg((void *) ph->p_va,
				(const uint8_t *) elf_header + ph->p_offset,
				ph->p_filesz, ph->p_memsz);

	// store the entry point from the ELF header
	*entry_point = elf_header->e_entry;
}

// Copy 'filesz' bytes starting at address 'data' into virtual address 'dst',
// then clear the memory from 'va+filesz' up to 'va+memsz' (set it to 0).
static void
copyseg(void *dst, const uint8_t *src, uint32_t filesz, uint32_t memsz)
{
	uint32_t va = (uint32_t) dst;
	uint32_t end_va = va + filesz;
	memsz += va;
	va &= ~(PAGESIZE - 1);		// round to page boundary

	// copy data
	memcpy((uint8_t *) va, src, end_va - va);

	// clear bss segment
	while (end_va < memsz)
		*((uint8_t *) end_va++) = 0;
}

static void
loader_panic(void)
{
	*((uint16_t *) 0xB8000) = '!' | 0x700;
    loop: goto loop;
}
