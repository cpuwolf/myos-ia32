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
#define	NR_PAGE		30

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

static kpage_t kmem_page[NR_PAGE];/*used by Kmalloc()*/
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
	if(size==0)return 0;
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
				return (void *)t->base;
			}
			else if(i->size==size)/*change its status*/
			{
				i->status=STAT_BUSY;
				return (void *)i->base;
			}
		}
	}
	return NULL;
}

void Kmem_free(void * base)
{
	struct hole * i,*pre,*t;
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
}
/*show mem */
void mem_show()
{
	struct hole *t;
	for(t=h_head;t!=NULL;t=t->next)
	{
		printf("0x%x-0x%x:[%d] size:%d\n",t->base,t->base+t->size-1,t->status,t->size);
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

static inline void __init kmem_page_init(char * kbase)
{
	int i;
	for(i=0;i<NR_PAGE;i++)
		kmem_page[i]=PAGE_S_CONT;
	kmem_page[0]=PAGE_S_FREE;
}

void * kmalloc(unsigned int size)
{
	int count,tmp_c;
	kpage_t * pptr=kmem_page,*tmp_p;
	count=size/PAGE_SIZE;
	if((size%PAGE_SIZE)>0)count++;
	
	for(; pptr<=(kmem_page+NR_PAGE) ;pptr++);
		if(*pptr==PAGE_S_FREE)
		{
			tmp_c=1;
			tmp_p=pptr;
			if(*pptr==PAGE_S_CONT)
				tmp_c++;
			if(tmp_c==count)
				return kmem_base+((tmp_p-kmem_page)/sizeof(kpage_t))*PAGE_SIZE;
		}
	return NULL;
}

void kfree(void * base)
{
	int index,count,i;
	unsigned int flags;
	kpage_t * pptr=kmem_page;
	
	index=((unsigned int)base-(unsigned int)kmem_base)/PAGE_SIZE;
	
	lock_irq_save(flags);
	count=*(pptr+index)/PAGE_SIZE;
	if((*(pptr+index)%PAGE_SIZE)>0)count++;
	
	for(i=index;i<=count;i++)
		*(pptr+i)=PAGE_S_CONT;
	*pptr=PAGE_S_FREE;	
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
	
	kmem_base=(char *)Kmem_alloc(PAGE_SIZE*NR_PAGE);
	kmem_page_init(kmem_base);
}