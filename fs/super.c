/* 
*	written by wei shuai
*	2004/9/26
*/
#include <hd.h>
#include <proto.h>
#include <buf.h>
#include <system.h>
#include <wait.h>
#include <fs.h>

#define NR_SUPER 2

struct super_block super_block[NR_SUPER];

static void super_cache_init()
{
	struct super_block * sb;
	for(sb=super_block;sb<(super_block+NR_SUPER);sb++)
	{
		INIT_WAIT_QUEUE_HEAD(sb->wait);
	}
}

static void wait_on_super(struct super_block * sb)
{
	unsigned int flags;
	lock_irq_save(flags);
	while(sb->lock)
		sleep_on(&(sb->wait));
	unlock_irq_restore(flags);
}

static void lock_super(struct super_block * sb)
{
	unsigned int flags;
	lock_irq_save(flags);
	while(sb->lock)
		sleep_on(&(sb->wait));
	sb->lock=1;
	unlock_irq_restore(flags);
}

static void unlock_super(struct super_block * sb)
{
	unsigned int flags;
	lock_irq_save(flags);
	sb->lock=0;
	wake_up(&(sb->wait));
	unlock_irq_restore(flags);
}

inline static unsigned int FstSecofDataClu(struct super_block * bd,unsigned int n)
{
	return ((n-2)*bd->cluster_size)+bd->data_first_sec;
}

struct super_block * get_empty_super()
{
	struct super_block * s;
	s=super_block;
	while(s<super_block+NR_SUPER)
		if(!s->dev)
				return s;
	return NULL;
}

struct super_block * get_super(int dev)
{
	struct super_block * s;

	if(!dev)
		return NULL;
	printk("look for super\n");
s_repeat:
	s=super_block;
	while(s<super_block+NR_SUPER)
	{
		if(s->dev==dev)	
		{
			wait_on_super(s);
			if(s->dev==dev)
				return s;
			goto s_repeat;
		}else
			s++;
	}
	return NULL;
}
void put_super(struct super_block * sb)
{
	lock_super(sb);
	sb->dev=0;
	unlock_super(sb);	
}

static struct super_block * read_super(int dev,struct ide_drive * ide)
{
	struct boot_sector * bs;
	struct super_block * bd;
	struct buf * bp=NULL;
	unsigned char fat_t[3]={12,16,32};
	unsigned int rootdirsectors,rootentcnt,bytspersec,datasec,countofclusters;
	unsigned int t_fatsz16,t_fatsz32,t_totsec16,t_totsec32,fatsz,totsec;
	
	if((bd=get_super(dev))!=NULL)
		return bd;
	if(!(bd=get_empty_super()))
		return NULL;
	bd->base=ide->table[0].first_log;
	bp=get_block(bd->base,512);
	if(!bp)
		panic("cannot read block");
	bs=(struct boot_sector *)(bp->data);
	/*determin it is fat 12 or 16 or 32*/
	rootentcnt=*(unsigned short *)bs->dir_number;
	bytspersec=*(unsigned short *)bs->sector_size;
	if(bytspersec!=512)
	{
		panic("Error Sector size\n");
	}
	t_fatsz16=bs->fatsz16;
	t_fatsz32=bs->u.fat32.fatsz32;
	t_totsec16=*(unsigned short *)bs->totsec16;
	t_totsec32=*(unsigned int *)bs->totsec32;
	/*count of sectors occupied by root dir*/
	rootdirsectors=((rootentcnt*32)+(bytspersec-1))/bytspersec;
	if(t_fatsz16!=0)
		fatsz=t_fatsz16;
	else
		fatsz=t_fatsz32;
	
	lock_super(bd);
	bd->dev=1;
	bd->data_first_sec=bd->base+bs->reserved+(bs->fat_number*fatsz)+rootdirsectors;
	bd->cluster_size=bs->cluster_size;	
	if(t_totsec16!=0)
		totsec=t_totsec16;
	else
		totsec=t_totsec32;
	/* count of sectors in data region*/
	datasec=totsec-(bs->reserved+(bs->fat_number*fatsz)+rootdirsectors);
	/*count of clusters*/
	countofclusters=datasec/bs->cluster_size;
	if(countofclusters<4085)/*fat12*/
	{
		bd->type=1;
		bd->dir_first_sec=bd->base+bs->reserved+(bs->fat_number*fatsz);
	}
	else if(countofclusters<65525)/*fat16*/
	{
		bd->type=2;
		bd->dir_first_sec=bd->base+bs->reserved+(bs->fat_number*fatsz);
	}
	else 
	{
		bd->type=3;/*fat32*/
		bd->dir_first_sec=FstSecofDataClu(bd,bs->u.fat32.rootclus);
	}
	/*type have been determined*/
	printk("File System:Fat%d\n",fat_t[bd->type-1]);
	bd->resvdseccnt=bs->reserved;
	bd->sector_size=bytspersec;		
	bd->fat_number=bs->fat_number;	
	bd->dir_number=rootentcnt;	/*needed update*/
	bd->sector_number=totsec;			
	bd->sectorsperfat=fatsz;	
	bd->sectorspertrack=bs->sectorspertrack;	
	bd->heads=bs->heads;		
	bd->hidden=bs->hidden;
	unlock_super(bd);
	put_block(bp);
	return bd;
}

void mount_fs(struct ide_drive * dev)
{
//	struct buf * bp=NULL;
	struct inode * rip=NULL;
	struct super_block * sp=NULL;
//	sp->base=dev->table[0].first_log;
	super_cache_init();
//	bp=get_block(sp->base,512);
//	if(!bp)
//			panic("cannot read block");
//	get_super(bp->data,sp);
	sp=read_super(1,dev);
	if(!sp)
			panic("cannot get super");
	rip=get_inode();
	lock_super(sp);
	sp->rootdir=rip;
	unlock_super(sp);
	rip->block[0]=sp->dir_first_sec;
	rip->size=sp->dir_number*32;
	rip->i_sb=sp;
//	put_block(bp);
}
