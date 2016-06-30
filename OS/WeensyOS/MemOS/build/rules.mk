OBJDIR := obj
comma = ,

# Cross-compiler toolchain
CC	= $(GCCPREFIX)gcc
CXX	= $(GCCPREFIX)c++
AS	= $(GCCPREFIX)as
AR	= $(GCCPREFIX)ar
LD	= $(GCCPREFIX)ld
OBJCOPY	= $(GCCPREFIX)objcopy
OBJDUMP	= $(GCCPREFIX)objdump
NM	= $(GCCPREFIX)nm
STRIP	= $(GCCPREFIX)strip

# Native commands
HOSTCC	= gcc
TAR	= tar
PERL	= perl

# LAB
LAB=5
LAB_NAME= lab$(LAB)

CIMS_QEMU_PATH	= /usr/local/pkg/qemu/1.7.0/bin/

# Check for i386-jos-elf compiler
ifndef GCCPREFIX
GCCPREFIX := $(shell if i386-jos-elf-objdump -i 2>&1 | grep '^elf32-i386$$' >/dev/null 2>&1 && i386-jos-elf-gcc -E -x c /dev/null >/dev/null 2>&1; \
	then echo 'i386-jos-elf-'; \
	elif objdump -i 2>&1 | grep 'elf32-i386' >/dev/null 2>&1; \
	then echo ''; \
	else echo "***" 1>&2; \
	echo "*** Error: Couldn't find an i386-*-elf version of GCC/binutils." 1>&2; \
	echo "*** Is the directory with i386-jos-elf-gcc in your PATH?" 1>&2; \
	echo "*** If your i386-*-elf toolchain is installed with a command" 1>&2; \
	echo "*** prefix other than 'i386-jos-elf-', set your GCCPREFIX" 1>&2; \
	echo "*** environment variable to that prefix and run 'make' again." 1>&2; \
	echo "*** To turn off this error, run 'gmake GCCPREFIX= ...'." 1>&2; \
	echo "***" 1>&2; exit 1; fi)
endif

# Compiler flags
# -Os is required for the boot loader to fit within 512 bytes;
# -ffreestanding means there is no standard library.
CPPFLAGS := $(DEFS) -I. -nostdinc
CFLAGS := $(CFLAGS) \
	-std=gnu99 -m32 -ffunction-sections \
	-ffreestanding -fno-omit-frame-pointer \
	-Wall -Wno-format -Wno-unused -Werror -ggdb -gdwarf-2
# Include -fno-stack-protector if the option exists.
CFLAGS += $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
DEPCFLAGS = -MD -MF $(DEPSDIR)/$*.d -MP

# Linker flags
LDFLAGS := $(LDFLAGS) -Os --gc-sections
# Link for 32-bit targets if on x86_64.
LDFLAGS	+= $(shell $(LD) -m elf_i386 --help >/dev/null 2>&1 && echo -m elf_i386)


# Dependencies
DEPSDIR := .deps
BUILDSTAMP := $(DEPSDIR)/rebuildstamp
DEPFILES := $(wildcard $(DEPSDIR)/*.d)
ifneq ($(DEPFILES),)
include $(DEPFILES)
endif

ifneq ($(DEP_CC),$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPCFLAGS) $(O) _ $(LDFLAGS))
DEP_CC := $(shell mkdir -p $(DEPSDIR); echo >$(BUILDSTAMP); echo "DEP_CC:=$(CC) $(CPPFLAGS) $(CFLAGS) $(DEPCFLAGS) $(O) _ $(LDFLAGS)" >$(DEPSDIR)/_cc.d; echo "DEP_PREFER_GCC:=$(PREFER_GCC)" >>$(DEPSDIR)/_cc.d)
endif

BUILDSTAMPS = $(OBJDIR)/stamp $(BUILDSTAMP)

$(OBJDIR)/stamp $(BUILDSTAMP):
	$(call run,mkdir -p $(@D))
	$(call run,touch $@)


# Qemu emulator
INFERRED_QEMU := $(shell if which qemu-system-i386 2>/dev/null | grep ^/ >/dev/null 2>&1; \
	then echo qemu-system-i386; \
	elif grep 16 /etc/fedora-release >/dev/null 2>&1; \
	then echo qemu; else echo qemu-system-i386; fi)
QEMU ?= $(INFERRED_QEMU)

QEMU_PATH = $(shell if which $(QEMU) 2>/dev/null | grep ^/ >/dev/null 2>&1; \
			then echo ""; \
			else echo $(CIMS_QEMU_PATH); fi)

GDBPORT := $(shell echo $$((20000+`id -u`%10000)) )

QEMUOPT	= -net none -parallel file:log.txt -k en-us

QEMUCONSOLE ?= $(if $(DISPLAY),,1)
QEMUDISPLAY = $(if $(QEMUCONSOLE),console,graphic)

QEMU_PRELOAD_LIBRARY = $(OBJDIR)/libqemu-nograb.so.1

$(QEMU_PRELOAD_LIBRARY): build/qemu-nograb.c
	$(call run,mkdir -p $(@D))
	-$(call run,$(HOSTCC) -fPIC -shared -Wl$(comma)-soname$(comma)$(@F) -ldl -o $@ $<)

QEMU_PRELOAD = $(shell if test -r $(QEMU_PRELOAD_LIBRARY); then echo LD_PRELOAD=$(QEMU_PRELOAD_LIBRARY); fi)


# Run the emulator

run: run-qemu-$(basename $(IMAGE))
run-qemu: run-qemu-$(basename $(IMAGE))
run-graphic: run-graphic-$(basename $(IMAGE))
run-console: run-console-$(basename $(IMAGE))
run-gdb: run-gdb-$(basename $(IMAGE))
run-gdb-graphic: run-gdb-graphic-$(basename $(IMAGE))
run-gdb-console: run-gdb-console-$(basename $(IMAGE))
run-graphic-gdb: run-gdb-graphic-$(basename $(IMAGE))
run-console-gdb: run-gdb-console-$(basename $(IMAGE))

.gdbinit: .gdbinit.tmpl
	sed "s/__gdb_port__/$(GDBPORT)/" < $^ > $@

check-qemu: $(QEMU_PRELOAD_LIBRARY) .gdbinit
	@if test -z "$$(which $(QEMU_PATH)$(QEMU) 2>/dev/null)"; then \
		echo 1>&2; echo "***" 1>&2; \
		echo "*** Cannot run $(QEMU). You may not have installed it yet." 1>&2; \
		echo "*** Try running this command to install it:" 1>&2; \
		echo sudo $$cmd qemu-system-x86 1>&2; \
		echo 1>&2; exit 1; \
	else :; fi

run-%: run-qemu-%
	@:

run-qemu-%: run-$(QEMUDISPLAY)-%
	@:

run-graphic-%: %.img kill check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -drive file=$<,index=0,media=disk,format=raw

run-console-%: %.img kill check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -curses -drive file=$<,index=0,media=disk,format=raw

run-gdb-%: run-gdb-graphic-%
	@:

run-gdb-graphic-%: %.img kill check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -S -gdb tcp::$(GDBPORT) -drive file=$<,index=0,media=disk,format=raw &
	-$(call run,gdb -x .gdbinit,GDB)

run-gdb-console-%: %.img kill check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -curses -S -gdb tcp::$(GDBPORT) -drive file=$<,index=0,media=disk,format=raw

# Kill all my qemus
kill:
	-killall -u $$(whoami) $(QEMU)
	@sleep 0.2; if ps -U $$(whoami) | grep $(shell temp=$(QEMU); echo $${temp:0:8}) >/dev/null; then killall -9 -u $$(whoami) $(QEMU); fi


# Delete the build
clean:
	$(call run,rm -rf $(DEPSDIR) $(OBJDIR) .gdbinit *.img core *.core,CLEAN)

realclean: clean
	$(call run,rm -rf $(DISTDIR)-handin.tgz $(DISTDIR)-handin)

# Distribute
DISTDIR = $(LAB_NAME)

distclean: realclean
	@/bin/rm -rf $(DISTDIR)


handin:
	$(call run,/bin/bash ./check-lab.sh . || false,CHECK LAB)
	$(call run,$(MAKE) LAB=$(LAB) -C .. handin,HANDIN)

# Boilerplate
always:
	@:

# These targets don't correspond to files
.PHONY: all always clean realclean distclean \
	run run-qemu run-graphic run-console run-gdb \
	run-gdb-graphic run-gdb-console run-graphic-gdb run-console-gdb \
	check-qemu kill \
	run-% run-qemu-% run-graphic-% run-console-% \
	run-gdb-% run-gdb-graphic-% run-gdb-console-%

# Eliminate default suffix rules
.SUFFIXES:

# Keep intermediate files
.SECONDARY:

# Delete target files if there is an error (or make is interrupted)
.DELETE_ON_ERROR:

# But no intermediate .o files should be deleted
.PRECIOUS: %.o $(OBJDIR)/%.o $(OBJDIR)/%.full $(OBJDIR)/bootsector
