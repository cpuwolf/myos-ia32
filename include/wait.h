/*
 *
 * copyright(C)	wei shuai
 * 2004/9/3
 * 
 */
 
#ifndef _OS_WAIT_H
#define _OS_WAIT_H

 
#include <proc.h> 
#include <list.h>

typedef struct wait_queue
{
	struct proc * proc;
	struct list_head queue_list;
}wait_queue_t;

typedef struct wait_queue_head
{
	struct list_head queue_list;
}wait_queue_head_t;

#define DECLARE_WAIT_QUEUE(name,task) \
	wait_queue_t name={task,{NULL,NULL}}

#define DECLARE_WAIT_QUEUE_HEAD(name) \
	wait_queue_head_t name={{&(name).queue_list,&(name).queue_list}}

static inline void add_wait_queue(wait_queue_head_t * head,wait_queue_t * new)
{
	list_add(&head->queue_list,&new->queue_list);
}

static inline void remove_wait_queue(wait_queue_head_t * head,wait_queue_t * old)
{
	list_del(&old->queue_list);
}

#endif