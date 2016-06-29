OBJDIR	= obj
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

# Handin
HANDIN 	= python submit.py
LAB_NAME= lab1
KEY_FILE= ~/cs202-auth-*
KEY_URL = http://ec2-54-173-71-20.compute-1.amazonaws.com/reg

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
CFLAGS	:= $(CFLAGS) $(DEFS) $(LABDEFS) -Os -ffreestanding -fomit-frame-pointer -I. -MD -Wall -Wno-format -Wno-unused -Werror -ggdb -nostdinc
# Include -fno-stack-protector if the option exists.
CFLAGS	+= $(shell $(CC) -fno-stack-protector -E -x c /dev/null >/dev/null 2>&1 && echo -fno-stack-protector)
# Include -m32 if the option exists (x86_64).
CFLAGS	+= $(shell $(CC) -m32 -E -x c /dev/null >/dev/null 2>&1 && echo -m32)
ifdef SOL
CFLAGS	+= -DSOL=$(SOL)
endif

# Linker flags
LDFLAGS	:= $(LDFLAGS)
# Link for 32-bit targets if on x86_64.
LDFLAGS	+= $(shell $(LD) -m elf_i386 --help >/dev/null 2>&1 && echo -m elf_i386)


# This magic automatically generates makefile dependencies
# for header files included from C source files we compile,
# and keeps those dependencies up-to-date every time we recompile.
# See 'mergedep.pl' for more information.
$(OBJDIR)/.deps: $(wildcard $(OBJDIR)/*.d)
	$(call run,mkdir -p $(@D))
	$(call run,$(PERL) mergedep.pl $@ $^)

-include $(OBJDIR)/.deps
-include conf/date.mk

# Qemu emulator
INFERRED_QEMU := $(shell if which qemu-system-i386 2>/dev/null | grep ^/ >/dev/null 2>&1; \
	then echo qemu-system-i386; \
	elif grep 16 /etc/fedora-release >/dev/null 2>&1; \
	then echo qemu; else echo qemu-system-i386; fi)
QEMU ?= $(INFERRED_QEMU)

QEMU_PATH = $(shell if which $(QEMU) 2>/dev/null | grep ^/ >/dev/null 2>&1; \
			then echo ""; \
			else echo $(CIMS_QEMU_PATH); fi)

GDBPORT = 20000

QEMUOPT	= -net none -parallel file:log.txt -k en-us

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

run-qemu-%: run-graphic-%
	@:

#XXX hack
run-graphic-%: %.img check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -drive file=$<,index=0,media=disk,format=raw

run-console-%: %.img check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -curses -drive file=$<,index=0,media=disk,format=raw

run-gdb-%: run-gdb-graphic-%
	@:

run-gdb-graphic-%: %.img check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -S -gdb tcp::$(GDBPORT) -drive file=$<,index=0,media=disk,format=raw &
	-$(call run,gdb -x .gdbinit,GDB)

run-gdb-console-%: %.img check-qemu
	@/bin/echo "  QEMU $<"
	@$(QEMU_PRELOAD) $(QEMU_PATH)$(QEMU) $(QEMUOPT) -curses -S -gdb tcp::$(GDBPORT) -drive file=$<,index=0,media=disk,format=raw


# Kill all my qemus
kill:
	-killall -u $$(whoami) $(QEMU)
	@sleep 0.2; if ps -U $$(whoami) | grep $(QEMU) >/dev/null; then killall -9 -u $$(whoami) $(QEMU); fi


# For deleting the build
clean:
	$(call run,rm -rf $(OBJDIR) .gdbinit *.img core *.core,CLEAN)

realclean: clean
	$(call run,rm -rf $(DISTDIR)-handin.tgz $(DISTDIR)-handin)

distclean: realclean
	@/bin/rm -rf $(DISTDIR)


# Distribute lab1
DISTDIR = lab1

distdir:
	@/bin/rm -rf $(DISTDIR)
	mkdir $(DISTDIR)
	perl mklab.pl 1 0 $(DISTDIR) COPYRIGHT GNUmakefile bootstart.S elf.h mergedep.pl process.h p-procos-app.c p-procos-app2.c p-procos-app3.c lib.c lib.h boot.c kernel.c kernel.h k-loader.c link/shared.ld k-int.S x86.c const.h types.h x86.h answers.txt build/mkbootdisk.c build/rules.mk build/qemu-nograb.c build/functions.gdb submit.py .gdbinit.tmpl .gitignore
	mkdir -p $(DISTDIR)/conf
	echo >$(DISTDIR)/conf/date.mk "PACKAGEDATE="`date`

dist: distdir

tarball: distclean realclean
	$(call run,mkdir $(DISTDIR)-handin,CREATE $(DISTDIR)-handin.tgz)
	$(call run,$(TAR) cf - `ls -a | grep -v '^\.*$$' | grep -v '^$(DISTDIR)*'` | (cd $(DISTDIR)-handin && $(TAR) xf - ))
	$(call run,/bin/rm -rf `find $(DISTDIR)-handin -name CVS -print`)
	$(call run,/bin/rm -rf `find $(DISTDIR)-handin -name .svn -print`)
	$(call run,date > $(DISTDIR)-handin/tarballstamp)
	$(call run,$(TAR) cf - $(DISTDIR)-handin | gzip >$(DISTDIR)-handin.tgz)
	$(call run,/bin/rm -rf $(DISTDIR)-handin)

handin: tarball
	@if test -f $(KEY_FILE); \
		then \
		echo "  SUBMITTING ... "; \
		$(HANDIN) $(LAB_NAME) $(DISTDIR)-handin.tgz; \
		else \
		echo "Key file not found! Please register a key at $(KEY_URL),"; \
		echo "and put it under your home directory first."; \
		fi;

# Patch targets
# Create a patch from ../$(DISTDIR).tar.gz.
patch-extract-tarball:
	@test -r ../$(DISTDIR).tar.gz || (echo "***" 1>&2; \
	echo "*** Can't find '../$(DISTDIR).tar.gz'.  Download it" 1>&2; \
	echo "*** into my parent directory and try again." 1>&2; \
	echo "***" 1>&2; false)
	@(gzcat ../$(DISTDIR).tar.gz 2>/dev/null || zcat ../$(DISTDIR).tar.gz) | tar xf -

patch-check-date: patch-extract-tarball
	@pkgdate=`grep PACKAGEDATE $(DISTDIR)/conf/date.mk | sed 's/.*=//'`; \
	test "$(PACKAGEDATE)" = "$$pkgdate" || (echo "***" 1>&2; \
	echo "*** The ../$(DISTDIR).tar.gz tarball was created on $$pkgdate," 1>&2; \
	echo "*** but your work directory was expanded from a tarball created" 1>&2; \
	echo "*** on $(PACKAGEDATE)!  I can't tell the difference" 1>&2; \
	echo "*** between your changes and the changes between the tarballs," 1>&2; \
	echo "*** so I won't create an automatic patch." 1>&2; \
	echo "***" 1>&2; false)

patch.diff: patch-extract-tarball always
	@rm -f patch.diff
	@for f in `cd $(DISTDIR) && find . -type f -print`; do \
	if diff -u $(DISTDIR)/"$$f" "$$f" >patch.diffpart || [ "$$f" = ./boot/lab.mk ]; then :; else \
	echo "*** $$f differs; appending to patch.diff" 1>&2; \
	echo diff -u $(DISTDIR)/"$$f" "$$f" >>patch.diff; \
	cat patch.diffpart >>patch.diff; \
	fi; done
	@for f in `find . -name $(DISTDIR) -prune -o -name obj -prune -o -name "patch.diff*" -prune -o -name '*.rej' -prune -o -name '*.orig' -prune -o -type f -print`; do \
	if [ '(' '!' -f $(DISTDIR)/"$$f" ')' -a '(' "$$f" != ./kern/appkernbin.c ')' ]; then \
	echo "*** $$f is new; appending to patch.diff" 1>&2; \
	echo New file: "$$f" >>patch.diff; \
	echo diff -u $(DISTDIR)/"$$f" "$$f" >>patch.diff; \
	echo '--- $(DISTDIR)/'"$$f"'	Thu Jan 01 00:00:00 1970' >>patch.diff; \
	diff -u /dev/null "$$f" | tail +2 >>patch.diff; \
	fi; done
	@test -n patch.diff || echo "*** No differences found" 1>&2
	@rm -rf $(DISTDIR) patch.diffpart

diff: patch-check-date patch.diff

patch:
	@test -r patch.diff || (echo "***" 1>&2; \
	echo "*** No 'patch.diff' file found!  Did you remember to" 1>&2; \
	echo "*** run 'make diff'?" 1>&2; \
	echo "***" 1>&2; false)
	@x=`grep "^New file:" patch.diff | head -n 1 | sed 's/New file: //'`; \
	if test -n "$$x" -a -f "$$x"; then \
	echo "*** Note: File '$$x' found in current directory;" 1>&2; \
	echo "*** not applying new files portion of patch.diff." 1>&2; \
	echo "awk '/^New file:/ { exit } { print }' <patch.diff | patch -p0"; \
	awk '/^New file:/ { exit } { print }' <patch.diff | patch -p0; \
	else echo 'patch -p0 <patch.diff'; \
	patch -p0 <patch.diff; \
	fi


# Always build something
always:
	@:

# These are phony targets
.PHONY: all always clean realclean distclean dist distdir tarball handin \
	run run-qemu run-graphic run-gdb run-gdb-graphic run-graphic-gdb \
	run-console run-gdb-console run-console-gdb \
	diff patch patch-extract-tarball patch-check-date patch-make-diff

# Eliminate default suffix rules
.SUFFIXES:

# Delete target files if there is an error (or make is interrupted)
.DELETE_ON_ERROR:

# no intermediate .o files should be deleted
.PRECIOUS: %.o $(OBJDIR)/%.o $(OBJDIR)/%-bootsector
