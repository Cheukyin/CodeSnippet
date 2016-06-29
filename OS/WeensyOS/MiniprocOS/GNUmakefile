IMAGE = procos.img
all: $(IMAGE)

# '$(V)' controls whether the lab makefiles print verbose commands (the
# actual shell commands run by Make), as well as the "overview" commands
# (such as '+ cc lib/readline.c').
#
# For overview commands only, run 'make all'.
# For overview and verbose commands, run 'make V=1 all'.
V = 0
ifeq ($(V),1)
compile = $(CC) $(CPPFLAGS) $(CFLAGS) $(DEPCFLAGS) $(1)
link = $(LD) $(LDFLAGS) $(1)
run = $(1) $(3)
else
compile = @/bin/echo " " $(2) $< && $(CC) $(CPPFLAGS) $(CFLAGS) $(DEPCFLAGS) $(1)
link = @/bin/echo " " $(2) $(patsubst %.full,%,$@) && $(LD) $(LDFLAGS) $(1)
run = @$(if $(2),/bin/echo " " $(2) $(3) &&,) $(1) $(3)
endif

-include build/rules.mk

BOOT_OBJS = $(OBJDIR)/bootstart.o $(OBJDIR)/boot.o

KERNEL_OBJS = $(OBJDIR)/k-int.o $(OBJDIR)/kernel.o \
	$(OBJDIR)/x86.o $(OBJDIR)/k-loader.o \
	$(OBJDIR)/lib.o
KERNEL_LINKER_FILES = link/shared.ld

PROCESS_SRCS = $(wildcard p-*.c)
PROCESS_OBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(PROCESS_SRCS))
PROCESS_BINARIES = $(patsubst %.c,$(OBJDIR)/%,$(PROCESS_SRCS))
PROCESS_LINKER_FILES = link/shared.ld

PROCESS_LIB_OBJS = $(OBJDIR)/lib.o

# Generic rules for making object files

$(PROCESS_OBJS): $(OBJDIR)/%.o: %.c
	$(call run,mkdir -p $(@D))
	$(call compile,-DWEENSYOS_PROCESS -nostdinc -c $< -o $@,COMPILE)

$(OBJDIR)/boot.o: $(OBJDIR)/%.o: boot.c
	$(call run,mkdir -p $(@D))
	$(call compile,-nostdinc -c $< -o $@,COMPILE)

$(OBJDIR)/%.o: %.c
	$(call run,mkdir -p $(@D))
	$(call compile,-DWEENSYOS_KERNEL -nostdinc -c $< -o $@,COMPILE)

$(OBJDIR)/%.o: %.S
	$(call run,mkdir -p $(@D))
	$(call compile,-DWEENSYOS_KERNEL -nostdinc -c $< -o $@,ASSEMBLE)

$(OBJDIR)/bootsector: $(BOOT_OBJS)
	$(call link,-N -e start -Ttext 0x7C00 -o $@.out $^,LINK)
	$(call run,$(OBJDUMP) -S $@.out >$@.asm)
	$(call run,$(OBJCOPY) -S -O binary -j .text $@.out $@)

$(OBJDIR)/mkbootdisk: build/mkbootdisk.c
	$(call run,$(HOSTCC) -I. -o $(OBJDIR)/mkbootdisk,HOSTCOMPILE,build/mkbootdisk.c)

# kernel is linked at address 0x100000.
$(OBJDIR)/kernel: $(KERNEL_OBJS) $(KERNEL_LINKER_FILES) $(PROCESS_BINARIES)
	$(call link,-e multiboot_start -Ttext 0x100000 -o $@ $(KERNEL_OBJS) $(KERNEL_LINKER_FILES) -b binary $(PROCESS_BINARIES),LINK)
	$(call run,$(OBJDUMP) -S $@ >$@.asm)
	$(call run,$(NM) -n $@ >$@.sym)

# processes are linked at address 0x200000.
$(PROCESS_BINARIES): %: %.o $(PROCESS_LIB_OBJS) $(KERNEL_LINKER_FILES)
	$(call link,-e pmain -Ttext 0x200000 -o $@ $^,LINK)
	$(call run,$(OBJDUMP) -S $@ >$@.asm)
	$(call run,$(NM) -n $@ >$@.sym)

procos.img: $(OBJDIR)/mkbootdisk $(OBJDIR)/bootsector $(OBJDIR)/kernel
	$(call run,$(OBJDIR)/mkbootdisk $(OBJDIR)/bootsector $(OBJDIR)/kernel > $@,CREATE $@)

/boot/procos: obj/kernel
	cp obj/kernel /boot/procos
