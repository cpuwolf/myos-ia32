# WRITTEN BY CPUWOLF 2003


all: system
 
OBJPRO=head.o main.o cpu.o console.o sys.o protect.o irq.o kb.o clock.o hd.o system.o proc.o pci.o lib.o fs.o mem.o schedule.o

system: $(OBJPRO) k.ld
	rm -f $@
	$(LD) $(LDFLAGS) -T ./k.ld -o $@ $(OBJPRO)
	
head.s: head.S

sys.s: sys.S

clean:
	rm -f *.o
	rm -f *.s
	rm -f system

include $(OSTOPDIR)/rules.make