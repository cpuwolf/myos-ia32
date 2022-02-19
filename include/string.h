/* 
*	written by wei shuai
*	2004/8/28
*/

#ifndef _OS_STRING_H
#define _OS_STRING_H

#define MYOS_INLINE  __attribute__((always_inline)) inline

static MYOS_INLINE char * strcpy(char * des,const char * src)
{
	int i,k;
	char * j;
	__asm__ __volatile__(\
	"1:\n\t"\
	"lodsb\n\t"\
	"stosb\n\t"\
	"test %%al,%%al\n\t"\
	"jne 1b\n\t"\
	:"=S"(i),"=D"(j),"=a"(k)\
	:"0"(src),"1"(des));
	return j;
}

static MYOS_INLINE int strcmp(const char * des,const char * src)
{
	int i,j,k;
	__asm__ __volatile__(\
	"1:\n\t"\
	"lodsb\n\t"\
	"scasb\n\t"\
	"jne 2f\n\t"/*not equal*/\
	"test %%al,%%al\n\t"\
	"jne 1b\n\t"/*check end of string*/\
	"xorl %%eax,%%eax\n\t"/*== return 0*/\
	"jmp 3f\n\t"\
	"2:\n\t"\
	"movl $1,%%eax\n\t"/*!= return 1*/\
	"3:"\
	:"=S"(i),"=D"(j),"=a"(k)\
	:"0"(src),"1"(des));
	return k;
}

#endif
