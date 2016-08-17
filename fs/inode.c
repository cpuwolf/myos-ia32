/* 
*	written by wei shuai
*	2004/9/26
*/
#include <type.h>
#include <fs.h>
#include <hd.h>
#include <proto.h>
#include <slot.h>
#include <system.h>

#define INODE_NR 20

static struct inode inode[INODE_NR];

FREE_SLOT_HEAD(inode_head);
DECLARE_SLOT_FUNC(inode,struct inode,free);

void inode_init()
{
	init_slot_inode(&inode_head,INODE_NR);
}

struct inode * get_inode()
{
	struct inode * ino;
	ino=alloc_slot_inode(&inode_head);
	if(!ino)
		return NULL;
	return ino;
}

void put_inode(struct inode * i)
{
	free_slot(&inode_head,&i->free);
}


static int FAToffset(struct super_block * bd,int n)
{
	int offset;
	if(bd->type==1)/*fat12*/
		offset=n+(n/2);
	else if(bd->type==2)/*fat16*/
		offset=n*2;
	else /*fat32*/
		offset=n*4;
	return offset;
}
static int eeof(struct super_block * bd,int val)
{
	if(bd->type==1)/*fat12*/
	{
		if(val>=0x0FF8)
			return 1;	
	}
	else if(bd->type==2)/*fat16*/
	{
		if(val>=0x0FFF8)
			return 1;
	}
	else /*fat32*/
	{
		if(val>=0x0FFFFFF8)
			return 1;
	}
	return 0;
}
static unsigned int FATEntryVal(struct super_block * bd,int cluster)
{
	unsigned int val,offset,sector;
	struct buf * secbuf;
	offset=FAToffset(bd,cluster);
	/*if we know cluster number N,where is the first sector*/
	sector=bd->resvdseccnt+(offset/bd->sector_size)+bd->base;
	secbuf=get_block(sector);
	if(secbuf==NULL)
	{
		printk("no kernel space\n");
		return 0;
	}
	offset=offset%bd->sector_size;
	if(bd->type==1)/*fat12*/
	{
		val=*((u16_t *)&(secbuf->data[offset]));
		if(cluster&0x0001)
			val=val>>4;/*ood*/
		else
			val=val&0xFFF;/*even*/
	}
	else if(bd->type==2)/*fat16*/
	{
		val=*((u16_t *)&(secbuf->data[offset]));
	}
	else/*fat32*/
	{
		val=(*((u32_t *)&(secbuf->data[offset])))&0x0FFFFFFF;
	}
	put_block(secbuf);
	return val;
}

void read_map(struct inode * ip)
{
	unsigned int index=10;
	int blk_size,count=0,i=0;
	
	blk_size=(ip->i_sb->cluster_size)*(ip->i_sb->sector_size);
	count=do_cdiv(ip->size,blk_size);
	/*if((ip->size%blk_size)>0)count++;*/
	index=ip->block[0];
	do
	{
		ip->block[i++]=index;
		index=FATEntryVal(ip->i_sb,index);
	}while((!eeof(ip->i_sb,index))&&(count-->0));
	
}