/*
 *
 * copyright(C)	wei shuai
 * 2004/9/3
 * 
 */
#ifndef _OS_LIST_H
#define _OS_LIST_H 
/* used by generic list*/ 
struct list_head {
	struct list_head *next,*prev;
};

#define list_entry(ptr, type, member) \
	((type *)((char *)(ptr)-(unsigned long)(&((type *)0)->member)))
	
	
/*insert a new entry after head*/
static inline void list_add(struct list_head * head,struct list_head *new)
{
	head->next->prev=new;
	new->next=head->next;
	new->prev=head;
	head->next=new;
}

/*insert a new entry before head*/
static inline void list_add_tail(struct list_head * head,struct list_head *new)
{
	head->prev=new;
	new->next=head;
	new->prev=head->prev;
	head->prev->next=new;
}

static inline void list_del(struct list_head * old)
{
	old->next->prev=old->prev;
	old->prev->next=old->next;
}

#endif