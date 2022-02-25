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

#define	IDLE_STACK	512
#define	INIT_STACK	512*100

static char userstack[IDLE_STACK+INIT_STACK];

static void idle_task()
{
idle:
	goto idle;
}

extern void alloc_segments(struct proc * p);

static void shell()
{	
	int i=0;
	//unsigned char buf[512];
idle1:
	i=kbread();
	if(i!=0)printf("%c",(unsigned char)i);
	//fread(buf,512,2);
	//_exit(0);
	goto idle1;
}

#define BUF_SIZE 10
static void init_task()
{
	unsigned char buf[BUF_SIZE];
	int file,size,i;
	int pid;
	
	fs_check();
	printf("file system check done\n\n");
	file=fopen("ZT.COM");
	if(file==-1)goto idle2;
	while(!feof(file))
	{
		size=fread(buf,1,BUF_SIZE,file);
		for(i=0;i<size;i++)
		{
			if(*(buf+i)!=0x0D)
				printf("%c",*(buf+i));
		}
	}
	printf("\n\n");
	
	memory();
	if((pid=fork())<0)
	{
		printf("Fork failed in init\n");
		goto idle2;
	}
	if(!pid)
	{
		printf("I am child\n");
		memory();
		shell();
	}
	else
		printf("I am parent,child pid=%d\n",pid);
idle2:
//	fread(buf,512,36);
//	printf("2");
	goto idle2;
}



/* retun a free proc slot */
struct proc * get_free_proc_struct()
{
	struct proc * rp;
	int t;
	for(t=0;t<NR_TASK;t++)
	{
			rp=(struct proc *)&proc_table[t];	
			if(!(rp->flags&P_IN_USE))
				return rp;
	}
	return NULL;
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
} tasktab[2]={\
{idle_task,IDLE_STACK},\
{init_task,INIT_STACK}};


/*when create a new process,setup its kernel thread*/
void proc_thread_init(struct proc * p)
{
	struct stackframe * p_regs;
	p_regs=GET_PROC_KSTACK(p);
	p->thread.esp0=(u32_t)(p_regs+1);
	p->thread.esp=(u32_t)p_regs;
	p->thread.eip=(u32_t)ret_with_schedule;
}

void __init proc_init()
{
		struct proc * rp;
		struct stackframe * p_reg;
		int t;
		u32_t tstkbase;
		ready_head=NULL;
		ready_tail=NULL;
		for(t=0;t<NR_TASK;t++)
		{
			rp=(struct proc *)&proc_table[t];
			rp->pid=t;
			rp->flags=0;
		}
		
		tstkbase=(u32_t)userstack;
		for(t=0;t<2;t++)
		{
			rp=(struct proc *)&proc_table[t];
			proc_thread_init(rp);
			proc_fd_init(rp);
			p_reg=GET_PROC_KSTACK(rp);		
			p_reg->eip=(u32_t)tasktab[t].init_eip;
			rp->map[T].phys=0;
			rp->map[T].vir=0;
			rp->map[T].size=0x20000;
			rp->map[D].phys=0;
			rp->map[D].vir=0;
			
			rp->map[S].phys=0;
			rp->map[S].vir=tstkbase;
			rp->map[S].size=tasktab[t].stksize;

			rp->map[D].size=rp->map[S].vir+rp->map[S].size;
			p_reg->esp=rp->map[S].vir+rp->map[S].size;	
			tstkbase+=tasktab[t].stksize;
			p_reg->psw=FLAGS_DEFAULT|IF;
			rp->pid=t;
			rp->flags|=P_IN_USE;
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

void * umap(struct proc * p,void * offset)
{
	/*check the offset to determine whether offset is in the segment*/
	if((u32_t)offset >= p->map[D].vir+p->map[D].size)
		return (void *)0;
	return (void *)(p->map[D].phys+(u32_t)offset);
}
