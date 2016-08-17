# WRITTEN BY CPUWOLF 2003

all: bootsect setup

bootsect: bootsect.o
	$(LD) $(LDFLAGS) -Ttext 0x0 -oformat binary -o $@ $<

setup: setup.o
	$(LD) $(LDFLAGS) -Ttext 0x0 -oformat binary -o $@ $<

bootsect.s: bootsect.S

bootsect.o: bootsect.s

setup.s: setup.S

setup.o: setup.s

clean:
	rm -f *.o
	rm -f bootsect
	rm -f setup
	rm -f *.s

include $(OSTOPDIR)/rules.make
