/* 
*	written by wei shuai
*	2004/5/23
*/
#include <type.h>
#include <proto.h>
#include <system.h>
#include <section.h>
#include <user.h>

#define STAT_BUSY 1
#define STAT_FREE 2


#define	NR_HOLE		30

#define	PAGE_SIZE	1024
#define	NR_PAGES		128

#define	PAGE_S_CONT	(~0)
#define	PAGE_S_FREE	0

static struct hole
{
	unsigned int base;
	unsigned int size;
	char status;
	struct hole * next; 
}mem_hole[NR_HOLE];

typedef unsigned int kpage_t;

static kpage_t kmem_page[NR_PAGES];/*used by Kmalloc()*/
static char * kmem_base;


static struct hole * h_head;
static struct hole * slot_free;	/*globle*/

/*system memory used by kernel and users(Bytes)*/
static unsigned int memory_start=0;
static unsigned int memory_end=0;


inline static void init_slot()
{
	struct hole * hp;
	/*put all mem_block into free slot list*/
	for(hp=&mem_hole[0];hp<&mem_hole[NR_HOLE];hp++)
		hp->next=hp+1;
	slot_free=mem_hole;
	mem_hole[NR_HOLE-1].next=NULL;	
}
static struct hole * alloc_slot()	/*like malloc,but it alloc fixed size*/
{
	register struct hole * ret=NULL;
	if(slot_free!=NULL)
	{
		ret=slot_free;
		slot_free=ret->next;
	}
	return ret;
}
static void free_slot(struct hole * h)	/*like free*/
{
	h->next=slot_free;	/*insert into head of the free list */
	slot_free=h;
}

void * Kmem_alloc(unsigned int size)
{
	struct hole * i,*t;
	unsigned int flags;
	if(size==0)return 0;
	
	lock_irq_save(flags);
	for(i=h_head;i!=NULL;i=i->next)
	{
		if(i->status==STAT_FREE)
		{
			if(i->size>size)/*split i,low for i,high for t*/
			{
				if((t=alloc_slot())==NULL)return NULL;
				t->next=i->next;
				t->base=i->base+size;
				t->size=i->size-size; 
				t->status=STAT_FREE;
				i->next=t;
				i->size=size;
				i->status=STAT_BUSY;
				unlock_irq_restore(flags);
				return (void *)i->base;
			}
			else if(i->size==size)/*change its status*/
			{
				i->status=STAT_BUSY;
				unlock_irq_restore(flags);
				return (void *)i->base;
			}
		}
	}
	unlock_irq_restore(flags);
	return NULL;
}

void Kmem_free(void * base)
{
	struct hole * i,*pre,*t;
	unsigned int flags;
	
	lock_irq_save(flags);
	for(i=h_head,pre=i;i!=NULL;i=i->next)
	{
		if(((u32_t)i->base==(u32_t)base)&&(i->status==STAT_BUSY))/*confirm the hole is used*/
		{
			i->status=STAT_FREE;
			/*look for a merge*/
			if(i->next!=NULL)/*check the next*/
			{
				if(i->next->status==STAT_FREE)
				{
					i->size=i->size+i->next->size;
					t=i->next;
					i->next=t->next;
					free_slot(t);
				}
			}
			if((pre->status==STAT_FREE)&&(pre!=i))/*check the previous*/
			{
				pre->size=pre->size+i->size;
				t=i->next;
				pre->next=t;
				free_slot(i);
				i=pre;
			}
			break;
		}
		pre=i;
	}
	unlock_irq_restore(flags);
}
/*show mem */
void mem_show()
{
	struct hole *t;
	for(t=h_head;t!=NULL;t=t->next)
	{
		printk("0x%x-0x%x:[%d] size:0x%x\n",t->base,t->base+t->size-1,t->status,t->size);
	}
}
/*memory unit check,write and read*/
/*static int mem_unit_check(void * addr,int value)
{
	register int i;
	__asm__ __volatile__("movb %b2,(%1)\n\t"\
			"movb (%1),%b0"\
			:"=r"(i)\
			:"r"((unsigned long)addr),"r"(value));
	return i;
}*/

/*we prepare kernel memory space for drivers and kernel itself*/
static inline void __init kmem_page_init()
{
	int i;
	for(i=0;i<NR_PAGES;i++)
		kmem_page[i]=PAGE_S_CONT;
	kmem_page[0]=PAGE_S_FREE;
}

inline static unsigned int calc_count(unsigned int size)
{
	int count;
	count=size/PAGE_SIZE;
	if((size%PAGE_SIZE)>0)count++;
	return count;
}
void * kmalloc(unsigned int size)
{
	int count,tmp_c=0,a_flag=0;
	unsigned int flags;
	kpage_t * pptr=kmem_page,*free_h=kmem_page;
	count=calc_count(size);
	lock_irq_save(flags);
	for(; pptr<=(kmem_page+NR_PAGES) ;pptr++)
	{
		if(*pptr==PAGE_S_FREE)
		{
			tmp_c=1;
			free_h=pptr;
			a_flag=1;
		}
		else if(*pptr==PAGE_S_CONT)
		{
			if(a_flag)
			{
				tmp_c++;	
			}
		}
		if(tmp_c==count)
		{
			*free_h=size;
			if(*(pptr+1)==PAGE_S_CONT)
				*(pptr+1)=PAGE_S_FREE;
			return (void *)((unsigned)kmem_base+(free_h-kmem_page)*PAGE_SIZE);
		}
	}
	unlock_irq_restore(flags);
	return NULL;
}

void kfree(void * base)
{
	int index;
	kpage_t *start=kmem_page,*end=kmem_page,*tmp;
	unsigned int flags;
	lock_irq_save(flags);
	index=((unsigned int)base-(unsigned int)kmem_base)/PAGE_SIZE;
	start=kmem_page+index;
	end=start+calc_count(*start);
	end--;
	
	if(start!=kmem_page)
	{
		tmp=start-1;
		while(*tmp==PAGE_S_CONT)tmp--;
		if(*tmp==PAGE_S_FREE)
			*start=PAGE_S_CONT;
		else
			*start=PAGE_S_FREE;	
	}else *start=PAGE_S_FREE;


	if(end!=(kmem_page+NR_PAGES))
	{
		if(*(end+1)==PAGE_S_FREE)
			*(end+1)=PAGE_S_CONT;
	}
	unlock_irq_restore(flags);	
}


void __init mem_init()
{
	/*I put memory size in 0x9000 when bootsect is running,get it now int KBytes*/
	unsigned long ext_mem_size;
	rep_movsb((unsigned int *)0x9000,&ext_mem_size,4);
	memory_end=(1<<20)+(ext_mem_size<<10);
	memory_start=1024*1024;
	printk("Extended Memory:%dKB(%dMB)\n",ext_mem_size,ext_mem_size/1024);
	init_slot();
	h_head=NULL;
	h_head=alloc_slot();
	h_head->base=memory_start;
	h_head->size=memory_end-memory_start+1;
	h_head->next=NULL;
	h_head->status=STAT_FREE;
	
	kmem_base=(char *)Kmem_alloc(PAGE_SIZE*NR_PAGES);
	if(!kmem_base)
			panic("cannot alloc for kernel");
	kmem_page_init();
}
