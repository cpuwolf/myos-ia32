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
	__asm__ __volatile__("int %1":"=a"(i):"ir"(SYS_VECTOR),"0"(0));
}
int kbread()
{
	register int i;
	__asm__ __volatile__("int %1":"=a"(i):"ir"(SYS_VECTOR),"0"(1));
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
		__asm__ __volatile__("int %1":"=a"(i):"ir"(SYS_VECTOR),"0"(2),"b"(stack));
}

int fopen(char * name)
{
	register int i;
	__asm__ __volatile__("int %1":"=a"(i):"ir"(SYS_VECTOR),"0"(3),"b"(name));
	return i;
}

int fread(void *buf,int size,int count,int fp)
{
	register int i;
	__asm__ __volatile__("int %1":"=a"(i):"ir"(SYS_VECTOR),"0"(4),"b"(buf),"c"(size),"d"(count),"S"(fp));
	return i;
}

int feof(int fp)
{
	register int i;
	__asm__ __volatile__("int %1":"=a"(i):"ir"(SYS_VECTOR),"0"(5),"b"(fp));
	return i;
}