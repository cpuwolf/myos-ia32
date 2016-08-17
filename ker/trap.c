/* 
*	written by cpuwolf
*	2005/3/29
*/
#include <proto.h>
#include <const.h>
#include <glob.h>
#include <proc.h>
#include <system.h>
#include <section.h>

static const char * exmsg[]={\
"Divide Error",\
"Debug",\
"NMI",\
"Breakpoint",\
"Overflow",\
"Bounds Exceeded",\
"Invalid Opcode",\
"Device not available",\
"Double Fault",\
"Coprocessor Segment Overrun",\
"Invalid TSS",\
"Segment Not Present",\
"Stack Fault",\
"General Protection",\
"Page-Fault",\
"reserved",\
"x87 FPU Floating-Point Error",\
"Alignment Check",\
"Machine-Check",\
"SIMD Floating-Point",\
};

void c_exception(int vec,unsigned int errorcode,struct stackframe * regs)
{
	int ext,ti,idt,select_index;
	lock();
	ext=errorcode&1;
	idt=errorcode&2;
	ti=errorcode&4;
	select_index=errorcode>>3;
	printk("%s Excepttion:%d\n",exmsg[vec],errorcode);
	if((regs->cs&3)==3)/*user mode error*/
	{
		printk("Pid:%d\n",current->pid);
		printk("ESP:0x%x:0x%x\n",regs->ss,regs->esp);
	}
	printk("EIP:0x%x:0x%x FLAGS:0x%x\n",regs->cs,regs->eip,regs->psw);
	printk("DS:0x%x ES:0x%x\n",regs->ds,regs->es);
	printk("EAX:0x%x EBX:0x%x ECX:0x%x EDX:0x%x\n",regs->eax,regs->ebx,regs->ecx,regs->edx);
	if(ext)
		printk("Event External\n");
	if(idt)
	{
		printk("Index:%d in IDT\n",select_index);
	}
	else if(ti)
		printk("Index:%d in LDT\n",select_index);
	else
		printk("Index:%d in GDT\n",select_index);
	__asm__ __volatile__("hlt\n\t");
}

