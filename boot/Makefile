# WRITTEN BY CPUWOLF 2003


all: bootsect

bootsect: bootsect.o
	$(LD) $(LDFLAGS) -Ttext 0x0 -oformat binary -o $@ $<

bootsect.s: bootsect.S

bootsect.o: bootsect.s

clean:
	rm -f *.o
	rm -f bootsect
	rm -f *.s

include $(OSTOPDIR)/rules.make