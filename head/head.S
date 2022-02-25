/* 
*	written by weishuai
*	2004/2
*/
#include <const.h>

.code32
.globl start32
.text
start32:
	
	movl	$DS_SELECTOR,%eax
	movl	%eax,%ds
	movl	%eax,%es
	movl	%eax,%ss
	movl	$0x9F000,%esp		/* init   segment reg 	*/

/*init bss memory zero*/
	cld	
	xorl	%eax,%eax
	movl	$_bss_start,%edi
	movl	$_bss_end,%ecx
	subl	%edi,%ecx
	rep
	stosb
	
	/* setup gdt,idt,tss*/
	call	prot_init

		
	ljmp	$CS_SELECTOR,$pro
pro:	movl	$DS_SELECTOR,%eax	/* reload all the segment registers*/
	movl	%eax,%ds		/* after changing gdt*/
	movl	%eax,%es
	movl	%eax,%fs
	movl	%eax,%gs
	
	movl	%eax,%ss
	movl	$0x09A000,%esp

	
/*disable int*/
	pushl	$0
	popfl
	
	call	start_kernel
	hlt
