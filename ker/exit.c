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

int do_exit(int error)
{
	struct proc * curr=current;
	if(!curr->pid)
		panic("Attempted to kill the idle Task");
	if(curr->pid==1)
		panic("Attempted to kill the Init Task");
	curr->flags&=(~P_IN_USE);
	Kmem_free((void *)curr->map[D].phys);
	curr->ldt_selector=0;
	curr->status=TASK_STOPED;
	schedule();
	return -1;
}

int sys_exit(int error)
{
	return do_exit(error);
}

