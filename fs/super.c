/* 
*	written by wei shuai
*	2004/9/26
*/
#include <fs.h>
#include <hd.h>
#include <proto.h>
#include <buf.h>

struct super_block super_block;

inline static unsigned int FstSecofDataClu(struct super_block * bd,unsigned int n)
{
	return ((n-2)*bd->cluster_size)+bd->data_first_sec;
}

static void get_super(unsigned char * buf,struct super_block * bd)
{
	struct boot_sector * bs;
	unsigned char fat_t[3]={12,16,32};
	unsigned int rootdirsectors,rootentcnt,bytspersec,datasec,countofclusters;
	unsigned int t_fatsz16,t_fatsz32,t_totsec16,t_totsec32,fatsz,totsec;
	bs=(struct boot_sector *)buf;
	/*determin it is fat 12 or 16 or 32*/
	rootentcnt=*(unsigned short *)bs->dir_number;
	bytspersec=*(unsigned short *)bs->sector_size;
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
}

void mount_fs(struct ide_drive * dev)
{
	struct buf * bp;
	struct inode * rip;
	struct super_block * sp=&super_block;
	sp->base=dev->table[0].first_log;
	bp=get_block(sp->base);
	get_super(bp->data,sp);
	rip=get_inode();
	sp->rootdir=rip;
	rip->block[0]=sp->dir_first_sec;
	rip->size=sp->dir_number*32;
	rip->i_sb=sp;
	put_block(bp);
}