# WRITTEN BY CPUWOLF 2003


all: fs.o
 
OBJPRO=super.o open.o read.o inode.o block.o filedes.o

fs.o: $(OBJPRO) 
	@rm -f $@
	$(LD) $(LDFLAGS) -r $(OBJPRO) -o $@

clean:
	rm -f *.o

include $(OSTOPDIR)/rules.make
