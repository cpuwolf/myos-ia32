#
#	written by weishuai 2003
#
#	modified by weishuai 2004/9
#
#	this is the main make


OSTOPDIR:=$(shell if [ "$$PWD" != "" ];then echo $$PWD;else pwd;fi)

CC=gcc
LD=ld
AS=as
AR=ar
CPP:=$(CC) -E
HPATH:=$(OSTOPDIR)/include
 
export CC LD AS CPP HPATH OSTOPDIR AR

CPPFLAGS:= -I$(HPATH)
CFLAGS:= -Wall -O2 $(CPPFLAGS) -m386
LDFLAGS:= -m elf_i386
AFLAGS:= $(CPPFLAGS)

export CPPFLAGS LDFLAGS CFLAGS AFLAGS

OBJCOPY=objcopy -O binary -R .note -R .comment -S

SUBDIRS=boot ker tools fs head init lib

CORE_FILES=ker/kernel.o fs/fs.o

LIB_FILES=lib/lib.a

all: fdImage


clean:
	@rm -f fdImage vmsys system.map vmkernel
	@make -C tools clean
	@make -C ker clean
	@make -C boot clean
	@make -C init clean
	@make -C head clean
	@make -C fs clean

fdImage: ossubdirs boot/bootsect boot/setup tools/build 
	$(LD) $(LDFLAGS) -T ./k.ld head/head.o init/main.o\
	$(CORE_FILES) \
	$(LIB_FILES) \
	-o vmkernel
	$(OBJCOPY) vmkernel vmsys
	./tools/build boot/bootsect boot/setup vmsys > fdImage
	nm -n vmkernel > system.map

ossubdirs: $(patsubst %,_dir_%,$(SUBDIRS))

$(patsubst %,_dir_%,$(SUBDIRS)): 
	make -C $(patsubst _dir_%,%,$@)
	
install: fdImage
	dd if=fdImage of=/dev/fd0 





