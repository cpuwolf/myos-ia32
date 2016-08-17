/*
 * written by wei shuai 
 * 2004/9/1
 */
#ifndef _OS_SECTION_H
#define _OS_SECTION_H

 
/*kernel init routing*/
#define __init		__attribute__ ((__section__ (".text.init")))

/*int service routing*/
#define __ISR		__attribute__ ((__section__ (".text.isr")))

#define FASTCALL __attribute__((regparm(3)))/*%eax,%edx,%ecx*/

#ifdef __ASSEMBLY__

#define ENTRY(name) \
	.globl name; \
	name:
#endif

#endif