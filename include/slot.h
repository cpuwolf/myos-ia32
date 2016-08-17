/* 
*	written by wei shuai
*	2004/8/31
*/
#ifndef _OS_SLOT_H
#define _OS_SLOT_H

#include <type.h>

struct free_list
{
	struct free_list * next; 
};


#define FREE_SLOT_HEAD(name) static struct free_list * name;

#define DECLARE_SLOT_FUNC(ptr,type,member) \
static void init_slot_##ptr(struct free_list ** head,int num) \
{ \
	int i; \
	for(i=0;i<num-1;i++)\
		(ptr+i)->member.next=&((ptr+i+1)->member);\
	*head=&(ptr->member);\
	(ptr+num-1)->member.next=NULL;	\
}\
static type * alloc_slot_##ptr(struct free_list ** head)\
{\
	struct free_list * ret;\
	if(*head!=0)\
	{\
		ret=*head;\
		*head=ret->next;\
	}else return NULL;\
	return ((type *)((char *)(ret)-(unsigned long)(&((type *)0)->member)));\
}\
\
static void free_slot(struct free_list ** head,struct free_list * h)\
{\
	h->next=*head;\
	*head=h;\
}


/*
example: using slot.h
struct mem
{
	int size;
	struct free_list free;
}mem[10];

FREE_SLOT_HEAD(free_head);
DECLARE_SLOT_FUNC(mem,struct mem,free);

int main(int argc, char* argv[])
{
	struct mem * mt;
	init_slot_mem(&free_head,10);
	mt=alloc_slot_mem(&free_head);
	free_slot(&free_head,&mt->free);
	return 0;
}
*/

#endif
