/* 
*	written by wei shuai
*	2004/3
*/
#include <type.h>
#include <proto.h>
#include <section.h>

void panic(const char * s)
{
		printk("Kernel panic:%s\n",s);
		for(;;);
}

