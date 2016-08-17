/*
 *
 * copyright(C)	wei shuai
 * 2004/4/9
 * 
 */
#ifndef _OS_SYSTEM_H
#define _OS_SYSTEM_H
 
#define load_LDT(n) __asm__ __volatile__("lldt %%ax"::"a"(n))
#define load_GDT(addr) __asm__ __volatile__("lgdt %0": "=m" (addr))
#define load_IDT(addr) __asm__ __volatile__("lidt %0": "=m" (addr))
#define load_TSS(sel) __asm__ __volatile__("ltr %%ax"::"a" (sel))

/* interrupt control */
#define unlock() __asm__ __volatile__("sti")
#define lock() __asm__ __volatile__("cli")

#define save_flags(x) __asm__ __volatile__("pushfl;popl %0":"=g"(x))
#define restore_flags(x) __asm__ __volatile__("pushl %0;popfl"::"g"(x))

#define lock_irq_save(x) __asm__ __volatile__("pushfl;popl %0;cli":"=g"(x))
#define unlock_irq_restore(x) restore_flags(x)



#define memcpy(f,t,n) rep_movsb((f),(t),(n))
#define memset(s,c,num) rep_stosb((c),(s),(num))

/*bytes copy*/
extern inline void rep_movsb(void *from, void *to,int bytes)
{
	int d0,d1,d2;
  	__asm__ __volatile__ (\
  	"cld\n\t"\
  	"rep;movsb\n\t"
  	:"=c"(d0),"=D"(d1),"=S"(d2)\
  	:"0" (bytes), "1" ((unsigned)to), "2" ((unsigned)from)\
  	:"memory");
}


/*memset*/
extern inline void rep_stosb(int value,void * dest,int numwords)
{
	int d1,d2;
	__asm__ __volatile__(\
	"cld\n\t"\
	"rep ; stosb\n\t"\
	:"=D"(d1),"=c"(d2)\
	:"a" (value),"0" ((unsigned)dest),"1" (numwords));
}




extern inline unsigned char in_byte (unsigned short port)
{
  register unsigned char data;
  __asm__ __volatile__ ("inb %w1,%b0":"=a" (data):"d" (port));
  return data;
}

extern inline unsigned char imm_in_byte (unsigned short port)
{
  register unsigned char data;
  __asm__ __volatile__ ("inb %w1,%b0":"=a" (data):"ir" (port));
  return data;
}

extern inline void out_byte (unsigned short port,unsigned char value)
{
  __asm__ __volatile__ ("outb %1,%w0"::"d" (port),"a"(value));
}

extern inline void imm_out_byte (unsigned short port,unsigned char value)
{
  __asm__ __volatile__ ("outb %1,%w0"::"ir" (port),"a"(value));
}

extern inline unsigned short in_word (unsigned short port)
{
  register unsigned short data;
  __asm__ __volatile__ ("inw %w1,%0":"=a" (data):"d" (port));
  return data;
}

extern inline void out_word (unsigned short port,unsigned short value)
{
  __asm__ __volatile__ ("outw %1,%w0"::"d" (port),"a"(value));
}


extern inline unsigned in_dword (unsigned short port)
{
  register unsigned data;
  __asm__ __volatile__ ("inl %w1,%0":"=a" (data):"d" (port));
  return data;
}

extern inline void out_dword (unsigned short port,unsigned value)
{
  __asm__ __volatile__ ("outl %1,%w0"::"d" (port),"a"(value));
}

extern inline int find_zero_bit(unsigned int * p)
{
	register unsigned int index=0;
	__asm__ __volatile__(\
		"bsfl %1,%0\n\t"\
		"jne 1f\n\t"\
		"movl $32,%0\n\t"\
		"1:\n\t"\
		:"=r"(index)\
		:"r"(~(*p)));
	return index;
}
/*set nr bit in *p,*p is a 32-bit Unit*/
extern inline void set_bit(unsigned int * p,int nr)
{
	__asm__ __volatile__(\
	"btsl %1,%0"\
	:"=m"(*p)\
	:"Ir"(nr)\
	);
}
/*reset nr bit in *p,*p is a 32-bit Unit*/
extern inline void clear_bit(unsigned int * p,int nr)
{
	__asm__ __volatile__(\
	"btrl %1,%0"\
	:"=m"(*p)\
	:"Ir"(nr)\
	);
}

extern inline unsigned int do_cdiv(unsigned int s,unsigned int d)
{
	unsigned int q,r;
	__asm__ __volatile__(\
	"div %3\n\t"\
	"testl %1,%1\n\t"\
	"je 1f\n\t"\
	"incl %0\n\t"\
	"1:\n\t"
	:"=a"(q),"=d"(r)\
	:"0"(s),"r"(d),"1"(0)\
	);
	return q;
}
#endif