#
# written by wei shuai 
#	2004/9/14


%.o: %.c 
	$(CC) $(CFLAGS) -nostdinc -fno-builtin-function -c $< -o $@

%.s: %.S 
	$(CPP)  -D__ASSEMBLY__ $(AFLAGS) -traditional $< -o $@

%.o: %.s 
	$(AS) $(AFLAGS) -o $@ $<
