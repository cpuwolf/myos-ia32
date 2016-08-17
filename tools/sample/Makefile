all: hello exam

ROOT=../..
LIBS=crt0.o $(ROOT)/lib/lib.a $(ROOT)/ker/vsprintf.o

exam: exam.c
	gcc -o $@ $<

hello: hello.o user.ld $(LIBS)
	ld -omagic -T user.ld $(LIBS) hello.o -o $@

hello.o: hello.c
	gcc -c $< -o $@

crt0.o:	crt0.s
	as $< -o $@

clean:
	@rm -f hello
	@rm -f exam
	@rm -f *.o
