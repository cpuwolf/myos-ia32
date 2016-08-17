/* 
*	written by wei shuai
*	2004/9/26
*/
#include <type.h>
#include <fs.h>
#include <hd.h>
#include <proto.h>
#include <slot.h>

#define FILP_NR 8

static struct file filp[FILP_NR];

FREE_SLOT_HEAD(filp_head);
DECLARE_SLOT_FUNC(filp,struct file,free);

void filp_init()
{
	init_slot_filp(&filp_head,FILP_NR);
}

struct file * get_filp()
{
	struct file * f;
	f=alloc_slot_filp(&filp_head);
	if(!f)
		return NULL;
	return f;
}

void put_filp(struct file * i)
{
	free_slot(&filp_head,&i->free);
}


