/*
*	2004-8-11 wei shuai
*/
/* globle variable*/

#ifndef _OS_GLOB_H
#define _OS_GLOB_H

#include <const.h>
#include <proc.h>
#include <protect.h>

extern struct proc * current;

extern struct tss_s tss;

extern struct pcb proc_table[NR_TASK];


#endif