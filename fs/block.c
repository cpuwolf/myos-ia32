/*
*	2004/9/27
*	Wei Shuai	(C)
*
*	read write block from disk
*/
#include <type.h>
#include <buf.h>
#include <proto.h>


struct buf * get_block(int block,int size)
{
	struct buf * bp;
	bp=(struct buf *)kmalloc(sizeof(struct buf));
	bp->data=(char *)kmalloc(size);
	if(bp==NULL)return NULL;
	bread(bp->data,size,block);
	return bp;
}


void put_block(struct buf * bp)
{
	kfree(bp->data);
	kfree(bp);
}
