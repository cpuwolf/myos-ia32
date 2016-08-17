/*
*	copyright(c)	
* 	wei shuai	2003/4
*/



#define SETUPSEG	0x1000			
#define BOOTSEG		0x07C0		
#define	INITSEG		0x9000 		
#define	SETUPSECTS	17		
#define	msglen		21					

.globl _start
.code16
.text
_start:
	movw	$BOOTSEG, %ax	/*move itself to 0x9000*/
	movw	%ax, %ds
	movw	$INITSEG, %ax
	movw	%ax, %es
	movw	$256, %cx
	subw	%si, %si
	subw	%di, %di
	cld
	rep
	movsw
	ljmp	$INITSEG,$go

/*NOW we are in  INITSEG*/

go:	movw	$0xA000, %sp		/*setup real-mode stack*/
	movw	%ax, %ds		
	movw	%ax, %ss
		
	movw	$0x0600,%ax
	movw	$0x0000,%cx
	movw	$0x184f,%dx
	movb	$7,%bh
	int 	$0x10		/*flush screen*/
	
	movb 	$0x02,%ah 	/*set cursor at 0:0*/
	xorw	%dx,%dx
	xorw	%bx,%bx
	int	$0x10
/*print 'The system is loading'*/
	lea	msg,%bp
	movw	$msglen,%cx
	movw	$0x0007,%bx
	movw	$0x1301,%ax
	int	$0x10
	
/*get floppy parameters(max sectors)*/
	xorb	%dl,%dl
	xorw	%ax,%ax
	int 	$0x13		/*reset fd*/
	
	movb	$0x08,%ah
	int	$0x13
	xorb	%ch,%ch
	movw	%cx,msectors	
	
/*now ! we read the floppy 0head,0track,2sect*/

read_it:	
	xorw	%ax,%ax
	xorb	%dl,%dl
	int 	$0x13
	
	movw	$SETUPSEG,%ax
	movw	%ax,%es
	movb	$0x02,%ah
	movb	$SETUPSECTS,%al
	movw	$0x0002,%cx
	movw	$0x0,%dx
	movw	$0x0,%bx
	int	$0x13

	jc	read_it 	#load failed!
	
read2:	
	movw	%ax,%cx
	shlw	$9,%cx
	addw	%cx,%bx
	movb	$0x1,%dh
	movb	$0x02,%ah
	movb	msectors,%al
	movw	$0x0001,%cx
	int	$0x13

	jnc	read2ok
	
	xorw	%ax,%ax
	xorb	%dl,%dl
	int 	$0x13
	jmp	read2
read2ok: 


/*get extended memory size int kb use e801*/
	movw	$0x0900,%ax
	movw	%ax,%gs
	movw	$0xe801, %ax
	int	$0x15
	jc	mem_skip

	andl	$0xffff, %edx			
	shll	$6, %edx			
	movl	%edx, %gs:(0x0)			
	andl	$0xffff, %ecx			
 	addl	%ecx, %gs:(0x0)
 	jmp 	mem_ok
mem_skip:
 	movl	$0, %gs:(0x0)	
mem_ok:
/*enable A20*/
	cli
	call 	empty_8042
	movb	$0xd1,%al
	outb	%al,$0x64
	call	empty_8042
	movb    $0xdf,%al
    	outb    %al,$0x60
	call    empty_8042
	sti


/*we prepare switch to protected mode	*/
	cli
	movw    %cs,%ax
    	movw    %ax,%ds
	lidt 	idt_table	
	lgdt	gdt_table

/*convert to protected mode */
	movl	$1,%eax
	movl	%eax,%cr0	
	jmp	flush
flush:
	jmpl	$0x08,$0x10000    		/*code prefix is 0x66*/
	
track:		.word 	0
head:		.word 	0
sector:		.word	0	
msectors:	.word 	0	/*max sectors of the floppy device*/
	
/*read_track:			#entry(%al is number of sectors)
	pushw	%ax
	pushw	%bx	
	movw 	$0x0e2e,%ax
	movw	$0x0007,%bx
	int	$0x10			#print '.'   0x2e='.'
	popw	%bx			
	popw	%ax
	
	movw	track,%dx
	movw	sector,%cx	#sector
	movb	%dl,%ch		#track
	movw	head,%dx
	movb	%dl,%dh		#head
	xorb	%dl,%dl		#device 0
	movb	$0x02,%ah
	int 	$0x13
	
	jnc	track_read_ok
	
	xorb	%dl,%dl
	xorw	%ax,%ax
	int 	$0x13		#reset fd
	jmp	read_track
track_read_ok:	
	ret
*/
empty_8042:
			inb 	$0x64,%al
			test	$2,%al
			jnz 	empty_8042	
			ret
			
msg:		.ascii	"The system is loading"

t_gdt:
	.word	0,0,0,0			# blank

	.word	0xFFFF			# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0			# base address = 0
	.word	0x9A00			# code read/exec
	.word	0x00CF			# granularity = 4096, 386
					#  (+5th nibble of limit)

	.word	0xFFFF			# 4Gb - (0x100000*0x1000 = 4Gb)
	.word	0			# base address = 0
	.word	0x9200			# data read/write
	.word	0x00CF

idt_table:      .word   0
        		.word   0,0

gdt_table:      .word   4*8-1
                .word   t_gdt,0x9	

.org 510
	.word 	0xAA55
