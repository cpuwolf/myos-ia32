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


struct pcb proc_table[NR_TASK];
struct proc * current;
struct proc * ready_head;
struct proc * ready_tail;


static void idle_task()
{
idle:
	goto idle;
}

extern void alloc_segments(struct proc * p);

static void idle_task2()
{	
	int i=0;
	/*unsigned char buf[512];*/
idle1:
	i=kbread();
	if(i!=0)printf("%c",(unsigned char)i);
	/*fread(buf,512,2);*/
	goto idle1;
}

static void try_task()
{
	/*unsigned char buf[512];*/
	fs_check();
	fopen("ZT.COM");
	mem_show();
idle2:
	/*fread(buf,512,36);*/
	/*printf("2");*/
	goto idle2;
}



/*
	ready a process.
	put the ready process on the tail of the ready queue
	it is a easy stuff
*/
void proc_ready(struct proc * rp)
{
	if((rp==IDLE_TASK)||(rp==NULL))return;
	if(ready_head!=NULL)
	{
		ready_tail->next=rp;
	}
	else
	{
		ready_head=rp;
	}
	rp->next=NULL;
	ready_tail=rp;
	
}



/* 
	block a process.
	if the process is on the head of the ready queue,directly remove it.
	Or else check all process in the ready queue ,and remove it
*/
void proc_block(struct proc * bp)
{
	struct proc * hp;
	hp=ready_head;
	if((bp==IDLE_TASK)||(bp==NULL))return;
	if(hp==bp)
	{
		ready_head=hp->next;
		current=pick_proc();
		return ;
	}
	while(hp->next!=bp)
	{
		hp=hp->next;
		if(hp==NULL)
		{
			return;
		}  
	}
	hp->next=hp->next->next;
	if(ready_tail==bp)
		ready_tail=hp;
}



/* pick a process for the head of the ready queue*/
struct proc * pick_proc()
{
	if(ready_head!=NULL)
	{
		return ready_head;
	}
	return IDLE_TASK;/*idle task*/
}


/* bill the current to check whether we need a schedule()*/
void bill_process_time()
{
	if(current==IDLE_TASK)
	{
		current->need_resched=1;
		return;
	}
	if(current->counter>0)
		current->counter--;
	else
		current->need_resched=1;
}

/*move current to the last of the run queue*/
void RoundRobin()
{
	if(ready_head==NULL)return;
	if(current->counter<=0)
	{
		ready_tail->next=ready_head;	/*move the head to the tail*/
		ready_tail=ready_head;
		ready_head=ready_head->next;
		ready_tail->next=NULL;
		current->counter=10;
	}
}
/* init task PCB used by main*/
struct tasktab
{
	task_ptr  init_eip;
	int	stksize;
} tasktab[NR_TASK]={\
{idle_task,100},\
{idle_task2,5*500},\
{try_task,10*500}};


/*when create a new process,setup its kernel thread*/
void proc_thread_init(struct proc * p)
{
	struct stackframe * p_regs;
	p_regs=(struct stackframe *)((u32_t)p+KERNEL_THREAD_SIZE)-1;
	p->thread.esp0=(u32_t)(p_regs+1);
	p->thread.esp=(u32_t)p_regs;
	p->thread.eip=(u32_t)ret_with_schedule;
}
void proc_fd_init(struct proc * p)
{
	p->files.openfd=0;
	p->files.maxopen=5;
}
void __init proc_init()
{
		struct proc * rp;
		struct stackframe * p_reg;
		int t,tstkbase,tstksize=0;
		ready_head=NULL;
		ready_tail=NULL;
		for(t=0;t<NR_TASK;t++)
			tstksize+=tasktab[t].stksize;
		tstkbase=(unsigned int)Kmem_alloc(tstksize);
		for(t=0;t<NR_TASK;t++)
		{
			rp=(struct proc *)&proc_table[t];
			proc_thread_init(rp);
			proc_fd_init(rp);
			p_reg=((struct stackframe *)((u32_t)rp+KERNEL_THREAD_SIZE))-1;		
			p_reg->eip=(u32_t)tasktab[t].init_eip;
			rp->p_map[T].mem_phys=0;
			rp->p_map[T].mem_vir=0;
			rp->p_map[T].mem_size=0x10000000;
			rp->p_map[D].mem_phys=0;
			rp->p_map[D].mem_vir=0;
			rp->p_map[D].mem_size=0x10000000;
			rp->p_map[S].mem_phys=0;
			rp->p_map[S].mem_vir=tstkbase;
			rp->p_map[S].mem_size=0x10000000;
			p_reg->esp=rp->p_map[S].mem_vir+tasktab[t].stksize-4;	
			tstkbase+=tasktab[t].stksize;
			p_reg->psw=FLAGS_DEFAULT|IF;
			rp->p_pid=t;
			rp->counter=10;
			rp->need_resched=0;
			rp->status=TASK_RUNNING;
			alloc_segments(rp);
			proc_ready(rp);
		}		
		current=IDLE_TASK;
		tss.esp0=current->thread.esp0;
		current->need_resched=1;
}

void * user_to_phys(struct proc * p,void * offset)
{
	/*check the offset to determine whether offset is in the segment*/
	if((u32_t)offset >= p->p_map[D].mem_vir+p->p_map[D].mem_size)
		return (void *)0;
	return (void *)(p->p_map[D].mem_phys+(u32_t)offset);
}