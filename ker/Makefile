# WRITTEN BY CPUWOLF 2003


all: kernel.o
 
OBJPRO=cpu.o console.o vsprintf.o sys.o protect.o \
irq.o trap.o kb.o clock.o acpi.o hd.o system.o proc.o pci.o mem.o \
schedule.o partition.o panic.o exit.o fork.o

kernel.o: $(OBJPRO) 
	rm -f $@
	$(LD) $(LDFLAGS) -r $(OBJPRO) -o $@
	

sys.s: sys.S

clean:
	rm -f *.o
	rm -f *.s
	rm -f system

include $(OSTOPDIR)/rules.make
