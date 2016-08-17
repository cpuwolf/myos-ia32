/* 
*	written by wei shuai
*	2004/5/19
*/
#include <type.h>
#include <proto.h>
#include <const.h>
#include <stdarg.h>

extern void vsprintf(char * buf,const char * fmt, va_list args);

void fs_check()
{
	register int i;
	__asm__ __volatile__("int %2":"=a"(i):"0"(0),"ir"(SYS_VECTOR));
}
int kbread()
{
	register int i;
	__asm__ __volatile__("int %2":"=a"(i):"0"(1),"ir"(SYS_VECTOR));
	return i;
}

void printf(const char * fmt, ...)
{
		char stack[100];/*alloc in stack*/
		int i;
		va_list args;
		va_start(args,fmt);
		vsprintf(stack,fmt,args);
		va_end(args);
		__asm__ __volatile__("int %3":"=a"(i):"0"(2),"b"(stack),"ir"(SYS_VECTOR));
}

int fopen(char * name)
{
	register int i;
	__asm__ __volatile__("int %3":"=a"(i):"0"(3),"b"(name),"ir"(SYS_VECTOR));
	return i;
}

int fread(void *buf,int size,int count,int fp)
{
	register int i;
	__asm__ __volatile__("int %6":"=a"(i):"0"(4),"b"(buf),"c"(size),"d"(count),"S"(fp),"ir"(SYS_VECTOR));
	return i;
}