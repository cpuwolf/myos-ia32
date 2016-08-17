/* 
*	written by weishuai
*	2004/2
*/

#include <proc.h>
#include <proto.h>
#include <section.h>
#include <glob.h>


#define _switch_to_first_bottom_half() \
	__asm__ __volatile__(\
		"movl %0,%%esp\n\t"\
		"jmp *%1\n\t"\
		::"r"(current->thread.esp),"r"(current->thread.eip));
		
void __init start_kernel(void)
{
		mem_init();/*check extended memory size,the conventional memory is 1mb*/
		scr_init();/* srceen const init*/
		intr_init();/* i8259 interrupt init*/
		printk("KITOS 1.0 Written By Wei Shuai\n");
		intel_init();/* check cpu*/
		proc_init();
		clock_init();/* 82C54 init*/
		kb_init();
		pci_init();
		hd_init();
		_switch_to_first_bottom_half();
		schedule();
}
