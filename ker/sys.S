/*
	copyright(C) wei shuai 2004/4
	modified in 2004/9 
*/

#include <const.h>
#include <frame.h>
#include <section.h>


#define	SAVE_ALL \
	cld;\
	pushl	%eax;\
	pushl	%ebx;\
	pushl	%ecx;\
	pushl	%edx;\
	pushl	%esi;\
	pushl	%edi;\
	pushl	%ebp;\
	pushl	%ds;\
	pushl	%es;\
	pushl	%fs;\
	pushl	%gs;

#define RESTORE_ALL \
	popl	%gs;\
	popl	%fs;\
	popl	%es;\
	popl	%ds;\
	popl	%ebp;\
	popl	%edi;\
	popl	%esi;\
	popl	%edx;\
	popl	%ecx;\
	popl	%ebx;\
	popl	%eax;\
	addl	$4,%esp;\
	iret;

#define GET_CURRENT(reg) \
	movl	%esp,reg;\
	addl	$(-FREE_STACK_SIZE),reg	
	
.text
ENTRY(save)	
	SAVE_ALL
	movw	$DS_SELECTOR,%dx			/*switch to kernel */	
	movw	%dx,%ds
	movw	%dx,%es
	
	movl	RETADDR(%esp),%eax
	call	do_IRQ

ret_from_int:	
	cli
	movb	CS(%esp),%bl
	testb	$3,%bl			/*switch to kernel?? */
	je	restore_all
	jmp	ret_with_schedule
		

/*
*
*		system call entry
*
*/	

ENTRY(system_call)
	cld
	pushl	%eax		/*	the retaddr*/
	SAVE_ALL
	/*GET_CURRENT*/
/* because current is not always equ to sys_caller ,...*/
	movw	%ss,%bp
	movw	%bp,%ds
	movw	%bp,%es
	sti			/*	enable int */
/*
*	function pass parameter
*	(0,	1,	2,	3,	4)
*	ebx,	ecx,	edx,	esi,	edi
*/
	pushl	%edi
	pushl	%esi
	pushl	%edx
	pushl	%ecx
	pushl	%ebx
	call	*sys_call_table(,%eax,4)
	addl	$4*5,%esp
	
	movl	%eax,EAX(%esp)		/*return value to user process	*/
/*now!inturrpt is disabled*/
ENTRY(ret_with_schedule)	
	cli
	movl	current,%ebp
	cmpl	$0,(%ebp)
	je	restore_all
	call	schedule
restore_all:
	RESTORE_ALL



/*	exception*/
ENTRY(divide_error)
	pushl	$0
	jmp	exception
ENTRY(debug_exception)
	pushl	$1
	jmp	exception
ENTRY(nmi)
	pushl	$2
	jmp	exception

ENTRY(breakpoint_exception)
	pushl	$3
	jmp	exception
ENTRY(overflow)
	pushl	$4
	jmp	exception
ENTRY(bounds_check)
	pushl	$5
	jmp	exception
ENTRY(inval_opcode)
	pushl	$6
	jmp	exception
ENTRY(copr_not_avaiable)
	pushl	$7
	jmp	exception
ENTRY(double_fault)
	pushl	$8
	jmp	errexception
ENTRY(copr_segment)
	pushl	$9
	jmp	exception
ENTRY(inval_tss)
	pushl	$10
	jmp	errexception
ENTRY(segment_not_present)
	pushl	$11
	jmp	errexception
ENTRY(stack_exception)
	pushl	$12
	jmp	errexception
ENTRY(general_protection)
	pushl	$13
	jmp	errexception
ENTRY(page_fault)
	pushl	$14
	jmp errexception
ENTRY(reserve)
	pushl	$15
	jmp exception
ENTRY(x87_fpu)
	pushl	$16
	jmp exception
ENTRY(align_check)
	pushl	$17
	jmp errexception
ENTRY(machine_check)
	pushl	$18
	jmp exception
ENTRY(simd)
	pushl	$19
	jmp exception

errexception:				/*	have a error code*/
	xchgl	%eax,4(%esp)			/*	ex_number*/
	xchgl	%ebx,(%esp)		/* error code */
	jmp	1f
exception:
	xchgl	%eax,(%esp)		/*get ex_number*/
	pushl	%eax
1:
	pushl   %ebx
    pushl   %ecx
    pushl   %edx
    pushl   %esi
    pushl   %edi
    pushl   %ebp
    pushl   %ds
    pushl   %es
    pushl   %fs
    pushl   %gs

	/*GET_CURRENT(%ebx)*/
	movl	%esp,%esi		/*%esi->struct stackframe pointer*/
	
	pushl	%esi			/*push struct stackframe pointer*/
	pushl	%ebx		/*push error code*/
	pushl	%eax		/*push exception number*/

	movl	$DS_SELECTOR,%edx
	movw	%dx,%ds
	movw	%dx,%es
	movw	%dx,%fs
	call	c_exception
	addl	$3*4,%esp
	
	movb	CS(%esp),%bl
	testb	$3,%bl			/*switch to kernel?? */
	jne	ret_with_schedule	
	jmp	restore_all

.data	
sys_call_table:
	.long	fs_init	/*0*/
	.long	kb_read		/*1*/
	.long	tty_write	/*2*/
	.long	sys_open	/*3*/
	.long 	sys_read	/*4*/
	.long	sys_feof	/*5*/
	.long	sys_exit	/*6*/
	.long	sys_fork	/*7*/
	.long	mem_show	/*8*/
