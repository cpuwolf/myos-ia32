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

#define save_halt() __asm__ __volatile__("sti;hlt")




/*bytes copy*/
extern inline void rep_movsb(void *from, void *to,int bytes)
{
  	__asm__ __volatile__ ("rep;movsb\n\t"::"c" (bytes), "D" ((unsigned)to), "S" ((unsigned)from));
}
/*words copy*/
extern inline void rep_movsw(void * src,void * dest,int words)
{
	__asm__ __volatile__("cld\n\t" "rep ; movsw"::"S" ((unsigned)src),"D" ((unsigned)dest),"c" (words));
}
/* dwords copy*/
extern inline void rep_movsl(void *from, void *to, unsigned dwords)
{
	__asm__ __volatile__ ("rep;movsl\n\t"::"c" (dwords), "D" (to), "S" (from));
}

extern inline void rep_stosw(int value,void * dest,int numwords)
{
	__asm__ __volatile__("cld\n\t" "rep ; stosw"::"a" (value),"D" ((unsigned)dest),"c" (numwords));
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


#endif