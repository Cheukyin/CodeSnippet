IMAGE = weensyos.img
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


# Object sets

BOOT_OBJS = $(OBJDIR)/bootstart.o $(OBJDIR)/boot.o

KERNEL_OBJS = $(OBJDIR)/k-exception.o $(OBJDIR)/kernel.o \
	$(OBJDIR)/k-hardware.o $(OBJDIR)/k-loader.o $(OBJDIR)/lib.o
KERNEL_LINKER_FILES = link/kernel.ld link/shared.ld

PROCESS_BINARIES = $(OBJDIR)/p-allocator $(OBJDIR)/p-allocator2 \
	$(OBJDIR)/p-allocator3 $(OBJDIR)/p-allocator4 \
	$(OBJDIR)/p-fork $(OBJDIR)/p-forkexit
PROCESS_LIB_OBJS = $(OBJDIR)/lib.o $(OBJDIR)/process.o
ALLOCATOR_OBJS = $(OBJDIR)/p-allocator.o $(PROCESS_LIB_OBJS)
PROCESS_OBJS = $(OBJDIR)/p-allocator.o $(OBJDIR)/p-fork.o \
	$(OBJDIR)/p-forkexit.o $(PROCESS_LIB_OBJS)
PROCESS_LINKER_FILES = link/process.ld link/shared.ld


# Generic rules for making object files

$(PROCESS_OBJS): $(OBJDIR)/%.o: %.c $(BUILDSTAMPS)
	$(call compile,-O1 -DWEENSYOS_PROCESS -c $< -o $@,COMPILE)

$(OBJDIR)/%.o: %.c $(BUILDSTAMPS)
	$(call compile,-DWEENSYOS_KERNEL -c $< -o $@,COMPILE)

$(OBJDIR)/boot.o: $(OBJDIR)/%.o: boot.c $(BUILDSTAMPS)
	$(call compile,-Os -fomit-frame-pointer -c $< -o $@,COMPILE)

$(OBJDIR)/%.o: %.S $(BUILDSTAMPS)
	$(call compile,-c $< -o $@,ASSEMBLE)


# Specific rules for WeensyOS

$(OBJDIR)/kernel.full: $(KERNEL_OBJS) $(PROCESS_BINARIES) $(KERNEL_LINKER_FILES)
	$(call link,-T $(KERNEL_LINKER_FILES) -nostdlib -o $@ $(KERNEL_OBJS) -b binary $(PROCESS_BINARIES),LINK)

$(OBJDIR)/p-%.full: $(OBJDIR)/p-%.o $(PROCESS_LIB_OBJS) $(PROCESS_LINKER_FILES)
	$(call link,-T $(PROCESS_LINKER_FILES) -nostdlib -o $@ $< $(PROCESS_LIB_OBJS),LINK)

$(OBJDIR)/p-allocator%.full: $(ALLOCATOR_OBJS) link/p-allocator%.ld link/shared.ld
	$(call link,-T link/p-allocator$*.ld link/shared.ld -nostdlib -o $@ $(ALLOCATOR_OBJS),LINK)

$(OBJDIR)/%: $(OBJDIR)/%.full
	$(call run,$(OBJDUMP) -S $< >$@.asm)
	$(call run,$(NM) -n $< >$@.sym)
	$(call run,$(OBJCOPY) -j .text -j .rodata -j .data -j .bss $<,STRIP,$@)

$(OBJDIR)/bootsector: $(BOOT_OBJS) link/boot.ld
	$(call link,-T link/boot.ld -nostdlib -o $@.full $(BOOT_OBJS),LINK)
	$(call run,$(OBJDUMP) -S $@.full >$@.asm)
	$(call run,$(NM) -n $@.full >$@.sym)
	$(call run,$(OBJCOPY) -S -O binary -j .text $@.full $@)

$(OBJDIR)/mkbootdisk: build/mkbootdisk.c $(BUILDSTAMPS)
	$(call run,$(HOSTCC) -I. -o $(OBJDIR)/mkbootdisk,HOSTCOMPILE,build/mkbootdisk.c)

weensyos.img: $(OBJDIR)/mkbootdisk $(OBJDIR)/bootsector $(OBJDIR)/kernel
	$(call run,$(OBJDIR)/mkbootdisk $(OBJDIR)/bootsector $(OBJDIR)/kernel > $@,CREATE $@)


/boot/weensyos: obj/kernel
	cp obj/kernel /boot/weensyos
