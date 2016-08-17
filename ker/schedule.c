/*
*	2004-8-11 wei shuai
*/

#include <proto.h>
#include <proc.h>
#include <wait.h>
#include <glob.h>
#include <system.h>
#include <section.h>


/*used for the kernel thread of the user process */
#define switch_kernel_thread(curr,next) \
	__asm__ __volatile__(\
		"pushl %%ecx\n\t"\
		"pushl %%esi\n\t"\
		"pushl %%edi\n\t"\
		"pushl %%ebp\n\t"\
		"movl %%esp,%0\n\t"		/*save esp in curr thread_struct*/\
		"movl %2,%%esp\n\t"		/*restore esp from next thread_struct*/\
		"movl $1f,%1\n\t"		/*save eip in curr thread_struct*/\
		"pushl %3\n\t"			/*save retaddr in next stack*/\
		"jmp _switch_to\n\t"	/*jmp! funy*/\
		"1:\n\t"\
		"popl %%ebp\n\t"\
		"popl %%edi\n\t"\
		"popl %%esi\n\t"\
		"popl %%ecx\n\t"\
		:"=m"(curr->thread.esp),"=m"(curr->thread.eip)\
		:"m"(next->thread.esp),"m"(next->thread.eip),\
		"a"(curr),"d"(next),"b"(curr));
		
/*pass param by registers*/		
void FASTCALL _switch_to(struct proc * curr,struct proc *next)
{
	tss.esp0=next->thread.esp0;
	load_LDT(next->ldt_selector);
}


void schedule()
{
	struct proc * curr,*next;
	curr=current;
/*rr policy*/	
	RoundRobin();

	switch(curr->status)
	{ 
		case TASK_INTERRUPT:
			proc_block(curr);
		default:
		case TASK_RUNNING:
	}
	curr->need_resched=0;	
/*
current interrupted by hardware(timer,hd),
calculate current->counter
*/
	next=pick_proc();
	/*'next' must have been selected*/
	if(curr!=next)
	{
		current=next;
		current->need_resched=0;
		switch_kernel_thread(curr,next);/*mutithread kernel*/
	}
}




/*syscall process kernel thread sleep*/
void sleep_on(wait_queue_head_t * head)
{
	unsigned int flags;
	DECLARE_WAIT_QUEUE(wait,current);
	lock_irq_save(flags);
	current->status=TASK_INTERRUPT;	
	add_wait_queue(head,&wait);
	unlock_irq_restore(flags);
	schedule();
	lock_irq_save(flags);
	remove_wait_queue(head,&wait);
	unlock_irq_restore(flags);
}


/*syscall process kernel thread wakeup*/
void wake_up_process(struct proc * p)
{
	unsigned int flags;
	lock_irq_save(flags);
	current->status=TASK_RUNNING;
	proc_ready(p);
	unlock_irq_restore(flags);
}

void wake_up(wait_queue_head_t * head)
{
	struct proc * p;
	struct list_head * tmp,*he;
	wait_queue_t * curr;
	unsigned int flags;
	if(!head)
		goto out;
	lock_irq_save(flags);
	he=&head->queue_list;
	tmp=he->next;
	while(tmp!=he)
	{
		curr=list_entry(tmp,wait_queue_t,queue_list);
		tmp=tmp->next;
		p=curr->proc;
		wake_up_process(p);
	}
	unlock_irq_restore(flags);
out:	
}


