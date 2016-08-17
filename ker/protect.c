/* 
*	written by cpuwolf
*	2004/2
*/
#include <type.h>
#include <const.h>
#include <protect.h>
#include <proc.h>
#include <glob.h>
#include <proto.h>
#include <section.h>
#include <system.h>


#define VECTOR(irq) (((irq)<8?IRQ0_VECTOR:IRQ8_VECTOR)+((irq)&0x07))

struct gatedesc idt[IDT_SIZE];
struct segdesc gdt[GDT_SIZE];
struct tss_s tss;

static void setseg(struct segdesc * segdp,unsigned base,unsigned size)
{
	segdp->base_low=base;
	segdp->base_middle=base>>16;
	segdp->base_high=base>>24;
	if(size>0xfffff)
	{
		segdp->limit_low=size>>12;
		segdp->granularity=0x80|(size>>28);
	}
	else
	{
		segdp->limit_low=size;
		segdp->granularity=size>>16;
	}
	segdp->granularity|=0x40;
						
}
void init_codeseg(struct segdesc * segdp,unsigned base,unsigned size,int pri)
{
	setseg(segdp,base,size);
	segdp->access=(pri<<5)|(PRESENT|EXECUTABLE|READABLE|SEGMENT);
}
void init_dataseg(struct segdesc * segdp,unsigned base,unsigned size,int pri)
{
	setseg(segdp,base,size);
	segdp->access=(pri<<5)|(PRESENT|WRITEABLE|SEGMENT);
}

static void init_gate(unsigned vector,unsigned base,unsigned p_dpl_type)
{
	struct gatedesc * idp;
	idp=&idt[vector];
	idp->offset_low=base;
	idp->selector=CS_SELECTOR;
	idp->p_dpl_type=p_dpl_type;
	idp->pad=0;
	idp->offset_high=base>>16;
}

/*alloc segment for process*/
void alloc_segments(struct proc * p)
{
	struct stackframe * p_reg;
	p_reg=((struct stackframe *)((u32_t)p+KERNEL_THREAD_SIZE))-1;
	init_codeseg(&p->ldt[CS_LDT_INDEX],(unsigned)p->p_map[T].mem_phys,(unsigned)p->p_map[T].mem_size,USER_PRIV);
	init_dataseg(&p->ldt[DS_LDT_INDEX],(unsigned)p->p_map[D].mem_phys,(unsigned)p->p_map[D].mem_size,USER_PRIV);
	init_dataseg(&p->ldt[SS_LDT_INDEX],(unsigned)p->p_map[S].mem_phys,(unsigned)p->p_map[S].mem_size,USER_PRIV);
	p_reg->cs=seg_selector(CS_LDT_INDEX,1,USER_PRIV);
	p_reg->ds=seg_selector(DS_LDT_INDEX,1,USER_PRIV);
	p_reg->es=seg_selector(DS_LDT_INDEX,1,USER_PRIV);
	p_reg->fs=seg_selector(DS_LDT_INDEX,1,USER_PRIV);
	p_reg->gs=seg_selector(DS_LDT_INDEX,1,USER_PRIV);
	p_reg->ss=seg_selector(SS_LDT_INDEX,1,USER_PRIV);
}
/*build ldt from process*/
inline void set_ldt_desc(struct segdesc * segdp,unsigned base,unsigned size,int pri)
{
	init_dataseg(segdp,base,size,pri);
	segdp->access=(PRESENT|LDT_TYPE);
}
/*build tss*/
inline void set_tss_desc(struct segdesc * segdp,unsigned base,unsigned size,int pri)
{
	init_dataseg(segdp,base,size,pri);
	segdp->access=(PRESENT|(KERNEL_PRIV<<5)|TSS_TYPE);
}

inline void set_int_gate(unsigned base,unsigned vector)
{
	init_gate(vector,base,(PRESENT|INT_TYPE));
}
inline void set_trap_gate(unsigned base,unsigned vector)
{
	init_gate(vector,base,(PRESENT|TRAP_TYPE));
}
inline void set_sys_gate(unsigned base,unsigned vector)
{
	init_gate(vector,base,(PRESENT|INT_TYPE|(USER_PRIV<<5)));
}
static void __init trap_init()
{
	set_trap_gate((u32_t)divide_error,0);
	set_trap_gate((u32_t)debug_exception,1);
	set_trap_gate((u32_t)nmi,2);
	set_trap_gate((u32_t)breakpoint_exception,3);
	set_trap_gate((u32_t)overflow,4);
	set_trap_gate((u32_t)bounds_check,5);
	set_trap_gate((u32_t)inval_opcode,6);
	set_trap_gate((u32_t)copr_not_avaiable,7);
	set_trap_gate((u32_t)double_fault,8);
	set_trap_gate((u32_t)copr_segment,9);
	set_trap_gate((u32_t)inval_tss,10);
	set_trap_gate((u32_t)segment_not_present,11);
	set_trap_gate((u32_t)stack_exception,12);
	set_trap_gate((u32_t)general_protection,13);
	/* system call */
	set_sys_gate((u32_t)system_call,SYS_VECTOR);
	/* interrupt gate*/
	set_int_gate((u32_t)hwint0,VECTOR(0));
	set_int_gate((u32_t)hwint1,VECTOR(1));
	set_int_gate((u32_t)hwint2,VECTOR(2));
	set_int_gate((u32_t)hwint3,VECTOR(3));
	set_int_gate((u32_t)hwint4,VECTOR(4));
	set_int_gate((u32_t)hwint5,VECTOR(5));
	set_int_gate((u32_t)hwint6,VECTOR(6));
	set_int_gate((u32_t)hwint7,VECTOR(7));
	set_int_gate((u32_t)hwint8,VECTOR(8));
	set_int_gate((u32_t)hwint9,VECTOR(9));
	set_int_gate((u32_t)hwint10,VECTOR(10));
	set_int_gate((u32_t)hwint11,VECTOR(11));
	set_int_gate((u32_t)hwint12,VECTOR(12));
	set_int_gate((u32_t)hwint13,VECTOR(13));
	set_int_gate((u32_t)hwint14,VECTOR(14));
	set_int_gate((u32_t)hwint15,VECTOR(15));
	
}
void __init prot_init(void)
{
	struct desctableptr	* dtp;
	int t;
	unsigned short ldt_index;
	struct proc * rp;
	dtp=(struct desctableptr *)&gdt[GDT_INDEX];
	*(u32_t *)dtp->base=(u32_t)gdt;
	*(u16_t *)dtp->limit=sizeof(gdt)-1;
	
	dtp=(struct desctableptr *)&gdt[IDT_INDEX];
	*(u32_t *)dtp->base=(u32_t)idt;
	*(u16_t *)dtp->limit=sizeof(idt)-1;
		
	/*the 2 descriptors are used by kernel*/
	init_codeseg(&gdt[CS_INDEX],0,0xFFFFFFFF,KERNEL_PRIV);
	init_dataseg(&gdt[DS_INDEX],0,0xFFFFFFFF,KERNEL_PRIV);
	
	/*build LDTs for each process*/
	for(t=0,ldt_index=FIRST_LDT_INDEX;t<NR_TASK;t++,ldt_index++)
	{
		rp=(struct proc *)&proc_table[t];
		set_ldt_desc(&gdt[ldt_index],(unsigned)rp->ldt,(unsigned)(sizeof(struct segdesc)*3),KERNEL_PRIV);
		rp->ldt_selector=seg_selector(ldt_index,0,KERNEL_PRIV);	
	}
	/*create a tss descriptor in GDT*/
	set_tss_desc(&gdt[TSS_INDEX],(unsigned)&tss,sizeof(tss),KERNEL_PRIV);
	tss.iobase=sizeof(tss);
	tss.ss0=DS_SELECTOR;
	trap_init();
	load_GDT(gdt[GDT_INDEX]);
	load_IDT(gdt[IDT_INDEX]);
	load_TSS(TSS_SELECTOR);
}

