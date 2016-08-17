/*
written by weishuai
2004/8/9
*/


#ifndef _OS_PROC_H
#define _OS_PROC_H

#include <type.h>
#include <protect.h>
#include <const.h>

#define	TASK_RUNNING 	1
#define	TASK_STOPED	2
#define	TASK_INTERRUPT	3


#define IDLE_TASK ((struct proc *)&proc_table[0])

struct stackframe
{
	u32_t	gs;		/* +0 */
	u32_t	fs;		/* +4 */
	u32_t	es;		/* +8 */
	u32_t	ds;		/* +12 */	
	u32_t 	ebp;		/* +16 */
	u32_t	edi;		/* +20 */
	u32_t 	esi;		/* +24 */	
	u32_t	edx;		/* +28 */
	u32_t 	ecx;		/* +32 */
	u32_t	ebx;		/* +36 */
	u32_t	eax;		/* +40 */
	u32_t	retaddr;	/* +44 */
	u32_t	eip;		/* +48 */
	u32_t	cs;		/* +52 */
	u32_t	psw;		/* +56 */
	u32_t	esp;		/* +60 */
	u32_t	ss;		/* +64 */
};


/* process memory map*/
struct memmap
{
	u32_t 	mem_vir;
	u32_t	mem_phys;
	u32_t	mem_size;
};

#define	T	0
#define	D	1
#define	S	2


struct thread
{
	u32_t esp0;
	u32_t eip;
	u32_t esp;	
};
/* process control block*/
struct proc
{
	unsigned need_resched;
	u16_t ldt_selector;
	struct segdesc ldt[3];
	struct thread thread;
	int p_pid;			/* process id*/
	int status;
	long counter;
	struct memmap p_map[3];	/* message map*/
	struct proc * next;	
};

struct pcb
{
	unsigned char kernel_stack[KERNEL_THREAD_SIZE];
};


#endif
