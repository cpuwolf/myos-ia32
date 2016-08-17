
/* 
*	written by wei shuai
*	2004/3
*/
#include <type.h>
#include <glob.h>
#include <proto.h>
#include <system.h>
#include <proc.h>
#include <section.h>
#include <user.h>


static int mem_copy(struct proc * p,struct proc * c)
{
	unsigned int r_size;
	u32_t child_base;
	unsigned int flags;
	
	r_size=p->map[S].size;
	r_size+=(p->map[S].vir-p->map[D].vir);
	lock_irq_save(flags);
	child_base=(u32_t)Kmem_alloc(r_size);
	unlock_irq_restore(flags);
	/* if there is no enough mem for new process */
	if(child_base==0)
		return -1;
	memcpy(p->map[D].phys,child_base,r_size);
	/* child vir is not changed */
	c->map[D].phys=child_base;
	c->map[S].phys=c->map[D].phys+(p->map[S].vir-p->map[D].vir);
	return 0;
}



/*fork process,return a pid*/
int do_fork()
{
	int pid;
	unsigned int flags;
	u16_t ldt_selector;
	struct proc * child,*father;
	struct stackframe * childregs,*fatherregs;
	
	/* look for a free struct proc */
	lock_irq_save(flags);
	child=get_free_proc_struct();
	unlock_irq_restore(flags);
	if(!child)
		return -1;
	father=current;
	
	ldt_selector=child->ldt_selector;
	pid=child->pid;
	*child=*father;
	child->pid=pid;
	child->ldt_selector=ldt_selector;
	
	childregs=GET_PROC_KSTACK(child);
	fatherregs=GET_PROC_KSTACK(father);
	*childregs=*fatherregs;
	childregs->eax=0;
	child->thread.esp0=(u32_t)(childregs+1);
	child->thread.esp=(u32_t)childregs;
	child->thread.eip=(u32_t)ret_with_schedule;
	if(mem_copy(father,child)==-1)
			return -1;
	child->flags|=P_IN_USE;
	child->counter=10;
	alloc_segments(child);
	wake_up_process(child);
	return pid;		
}

int sys_fork()
{
	return do_fork();
}
