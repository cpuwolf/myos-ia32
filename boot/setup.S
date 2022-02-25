/*
*	copyright(c)	
* 	wei shuai	2003/4
*/

#include <config.h>

SYSSEG=DEF_SYSSEG			
INITSEG=DEF_INITSEG
SETUPSEG=DEF_SETUPSEG




.globl _start
.code16
.text

_start:
	jmp		realmode_kernel
	.ascii	"WS"

realmode_kernel:
/*now CS==SETUPSEG,DS=CS*/
	movw	%cs,%ax
	movw	%ax,%ds
	movw	%ax,%es
	movw	%ax,%ss

/*check setup*/
	cmpw	$SIG1,sig1_check
	jne		bad_sig
	cmpw	$SIG2,sig2_check
	jne		bad_sig
	jmp		good_sig

bad_sig:	
/*print 'setup have been broken'*/
    lea msgsig,%bp
    movw    $25,%cx
    movw    $0x0200,%dx
    movw    $0x0007,%bx
    movw    $0x1301,%ax
    int $0x10
no_sig_loop:
	jmp		no_sig_loop

good_sig:

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
	xorl	%eax,%eax
	movw	%ds,%ax
	shll	$4,%eax
	addl	$t_gdt,%eax
	movl	%eax,(gdt_table+2)
	lgdt	gdt_table

/*convert to protected mode */
	movl	$1,%eax
	movl	%eax,%cr0	
	jmp	flush
flush:
/*	jmpl	$0x08,$0x10000    		code prefix is 0x66*/
	.byte 	0x66,0xea
	.long	0x10000
	.word	0x08
	
		
empty_8042:
			inb 	$0x64,%al
			test	$2,%al
			jnz 	empty_8042	
			ret
			

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
                .word   0,0

msg:	.ascii	"system setup check ok!"
msgsig:	.ascii	"setup have been broken!:("
msgswitch:	.ascii	"switch to protected mode"
.org 506
kernel_secs: .word	50
.org 508
sig1_check:	.word 	SIG1
sig2_check:	.word	SIG2
