/* 
*	written by wei shuai
*	2004/8/30
*/
#ifndef _OS_PROTECT_H
#define _OS_PROTECT_H

struct segdesc
{
	u16_t limit_low;
	u16_t base_low;
	u8_t base_middle;
	u8_t access;
	u8_t granularity;
	u8_t base_high;
};

struct desctableptr
{
	char limit[2];
	char base[4];
};
						
struct gatedesc
{
	u16_t offset_low;
	u16_t selector;
	u8_t pad;
	u8_t p_dpl_type;
	u16_t offset_high;
};


struct tss_s
{
	u32_t 	backlink;
	u32_t 	esp0;
	u32_t	ss0;
	u32_t 	esp1;
	u32_t 	ss1;
	u32_t 	esp2;
	u32_t	ss2;
	u32_t 	cr3;
	u32_t	eip;
	u32_t	flags;
	u32_t	eax;
	u32_t	ecx;
	u32_t	edx;
	u32_t	ebx;
	u32_t	esp;
	u32_t	ebp;
	u32_t	esi;
	u32_t	edi;
	u32_t	es;
	u32_t	cs;
	u32_t	ss;
	u32_t	ds;
	u32_t	fs;
	u32_t	gs;
	u32_t	ldt;
	u16_t	trap;
	u16_t	iobase;
	
};



/* protect mode const*/
#define PRESENT 	0x80
#define SEGMENT 	0x10
#define EXECUTABLE 	0x08
#define READABLE 	0x02
#define WRITEABLE 	0x02
#define ACCESSED 	0x01

#define TRAP_TYPE	0x0F
#define	INT_TYPE	0x0E
#define	LDT_TYPE	0x02
#define	TSS_TYPE	0x09

/*system flags*/
#define FLAGS_DEFAULT	0x0
#define IF		0x200

/*used by userprocess*/
#define CS_LDT_INDEX 	0
#define DS_LDT_INDEX 	1
#define SS_LDT_INDEX 	2


#define seg_selector(index,TI,RPL) ((index<<3)|(TI<<2)|RPL)

#define KERNEL_PRIV	0
#define USER_PRIV	3


#endif