#
# written by wei shuai 
#	2004/9/14


%.o: %.c 
	$(CC) $(CFLAGS) -nostdinc -fno-builtin-function -c $< -o $@

.S.s: 
	$(CPP)  -D__ASSEMBLY__ $(CPPFLAGS) -traditional $< -o $@

.s.o:  
	$(AS) $(AFLAGS) -o $@ $<
