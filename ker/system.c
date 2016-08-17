/*
 *
 * copyright(C)	wei shuai
 * 2004/4/9
 * 
 */

#include <const.h>

void in_words(unsigned short port,void * buf,int size)
{
	__asm__ __volatile__(\
	"cld\n\t"\
	"pushl	%%es\n\t"\
	"movw	%w0,%%es\n\t"\
	"rep\n\t"\
	"insw\n\t"\
	"popl	%%es"::"r"(DS_SELECTOR),"d"(port),"c"(size),"D"((unsigned)buf));
}