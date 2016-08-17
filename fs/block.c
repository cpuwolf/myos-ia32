/*
*	2004/9/27
*	Wei Shuai	(C)
*
*	read write block from disk
*/
#include <type.h>
#include <buf.h>
#include <proto.h>

struct buf * get_block(int block)
{
	struct buf * bp;
	bp=(struct buf *)kmalloc(sizeof(struct buf));
	if(bp==NULL)return NULL;
	ide_read(bp->data,BLOCK_SIZE,block);
	return bp;
}


void put_block(struct buf * bp)
{
	kfree(bp);
}