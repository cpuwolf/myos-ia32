/* 
*	written by weishuai
*	2004/2
*/
#include <type.h>
#include <proto.h>
#include <section.h>
#include <string.h>

static struct cpuinfo_x86 {
	u8_t	family;		/* CPU family */
	u8_t	vendor;	/* CPU vendor */
	u8_t	model;
	u8_t	mask;
	char	hard_math;
	char	rfu;
       	int	cpuid_level;	/* Maximum supported CPUID level, -1=no CPUID */
	char	vendor_id[16];
	char	model_id[50];
	int 	cache_size;  /* in KB - valid for CPUS which support this call  */
	int	fdiv_bug;
	int	f00f_bug;
	int	coma_bug;
	unsigned long loops_per_jiffy;
	unsigned long *pgd_quick;
	unsigned long *pmd_quick;
	unsigned long *pte_quick;
	unsigned long pgtable_cache_sz;
}cpuinfo;
struct cpu_model_info {
	int family;
	char * model_names[16];
};
static struct cpu_model_info cpu_models[]= {
	{5,
	  { "Pentium 60/66 A-step", "Pentium 60/66", "Pentium 75 - 200",
	    "OverDrive PODP5V83", "Pentium MMX", NULL, NULL,
	    "Mobile Pentium 75 - 200", "Mobile Pentium MMX", NULL, NULL, NULL,
	    NULL, NULL, NULL, NULL }},
	{6,
	  { "Pentium Pro A-step", "Pentium Pro", NULL, "Pentium II (Klamath)", 
	    NULL, "Pentium II (Deschutes)", "Mobile Pentium II",
	    "Pentium III (Katmai)", "Pentium III (Coppermine)", NULL,
	    "Pentium III (Cascades)", NULL, NULL, NULL, NULL }}
	    };


static char * __init model_table_search(struct cpuinfo_x86 *c)
{
	int i;
	struct cpu_model_info *info = cpu_models;
	if ( c->model >= 16 )
		return NULL;/* Range check */
		
	for(i=0;i<(sizeof(cpu_models)/sizeof(struct cpu_model_info));i++)
	{
		if(info->family==c->family)
			return info->model_names[c->model];
		info++;
	}
	return NULL;/* Not found */
}


/* asm cpuid instruction*/
extern inline void cpuid(int op,unsigned int *eax,unsigned int *ebx,unsigned int *ecx,unsigned int *edx)
{
	__asm__ __volatile__("cpuid\n\t":"=a"(*eax),"=b"(*ebx),"=c"(*ecx),"=d"(*edx):"0"(op));
}

/*cpu support 0x80000002~4*/
static void __init get_model_name(struct cpuinfo_x86 *c)
{
	unsigned int eax,ebx,ecx,edx;
	unsigned int *v;
	char * p,*q;
	
	cpuid(0x80000000,&eax,&ebx,&ecx,&edx);
	if(eax<0x80000004)
	{
		cpuid(1,&eax,&ebx,&ecx,&edx);
		c->family=(eax>>8)&0xf;
		c->model=(eax>>4)&0xf;
		strcpy(c->model_id,model_table_search(c));
		return ;
	}
	
	/*brand string supported*/	
	v=(unsigned int *)c->model_id;
	cpuid(0x80000002, &v[0], &v[1], &v[2], &v[3]);
	cpuid(0x80000003, &v[4], &v[5], &v[6], &v[7]);
	cpuid(0x80000004, &v[8], &v[9], &v[10], &v[11]);
	c->model_id[48] = 0;
	
	/* Intel chips right-justify this string for some dumb reason;
	   undo that brain damage */
	p = q = &c->model_id[0];
	while ( *p == ' ' )
	     p++;
	if ( p != q ) {
	     while ( *p )
		  *q++ = *p++;
	     while ( q <= &c->model_id[48] )
		  *q++ = '\0';	/* Zero-pad the rest */
	}	
	return ;
}
void __init intel_init()
{
	unsigned int maxvalue;
	
	/*get vendor name*/
	unsigned int * v=(unsigned int *)cpuinfo.vendor_id;
	cpuid(0,&maxvalue,&v[0],&v[1],&v[2]);
	cpuinfo.vendor_id[12]=0;
	
	get_model_name(&cpuinfo);
	
	printk("CPU %s:%s\n",cpuinfo.vendor_id,cpuinfo.model_id);
	/*printk("CPU brand index: %d\n",cpuverinfo.ebx&0xff);
	printk("CPU steping ID: %d\n",cpuverinfo.eax&0xf);
	printk("CPU model: %d\n",(cpuverinfo.eax>>4)&0xf);
	printk("CPU family: %d\n",(eax>>8)&0xf);
	printk("CPU type: %d\n",(cpuverinfo.eax>>12)&0x3);*/
}
