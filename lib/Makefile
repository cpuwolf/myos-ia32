# WRITTEN BY CPUWOLF 2003


all: lib.a
 
OBJPRO=lib.o

lib.a: $(OBJPRO) 
	rm -f $@
	$(AR) rcs $@ $(OBJPRO)
	

clean:
	rm -f *.o
	rm -f *.a

include $(OSTOPDIR)/rules.make
