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
CPP:=$(CC) -E
HPATH:=$(OSTOPDIR)/include
 
export CC LD AS CPP HPATH OSTOPDIR

CPPFLAGS:= -I$(HPATH)
CFLAGS:= -Wall -O2 $(CPPFLAGS) -m386
LDFLAGS:= -m elf_i386
AFLAGS:= $(CPPFLAGS)

export CPPFLAGS LDFLAGS CFLAGS AFLAGS

OBJCOPY=objcopy -O binary -R .note -R .comment -S

SUBDIRS=boot ker tools

all: fdImage


clean:
	@rm -f fdImage
	@rm -f vmsys
	@rm -f system.map
	@make -C tools clean
	@make -C ker clean
	@make -C boot clean

fdImage: ossubdirs ker/system boot/bootsect tools/build
	$(OBJCOPY) ker/system vmsys
	./tools/build boot/bootsect vmsys > fdImage
	nm -n ker/system > system.map

ossubdirs: $(patsubst %,_dir_%,$(SUBDIRS))

$(patsubst %,_dir_%,$(SUBDIRS)): 
	make -C $(patsubst _dir_%,%,$@)
	
install: fdImage
	dd if=fdImage of=/dev/fd0 





