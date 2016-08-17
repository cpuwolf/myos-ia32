/* 
*	written by wei shuai
*	2004/5/23
*/

#include <proto.h>
#include <system.h>
#include <fs.h>
#include <partition.h>
#include <string.h>

static struct super_block super_block;
static unsigned char data_buf[512];

static void get_superblock(unsigned char * buf,struct super_block * bd)
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
	t_fatsz32=bs->spec.fat32.fatsz32;
	t_totsec16=*(unsigned short *)bs->totsec16;
	t_totsec32=*(unsigned int *)bs->totsec32;
	/*count of sectors occupied by root dir*/
	rootdirsectors=((rootentcnt*32)+(bytspersec-1))/bytspersec;
	if(t_fatsz16!=0)
		fatsz=t_fatsz16;
	else
		fatsz=t_fatsz32;
		
	if(t_totsec16!=0)
		totsec=t_totsec16;
	else
		totsec=t_totsec32;
	/* count of sectors in data region*/
	datasec=totsec-(bs->reserved+(bs->fat_number*fatsz)+rootdirsectors);
	/*count of clusters*/
	countofclusters=datasec/bs->cluster_size;
	if(countofclusters<4085)/*fat12*/
		bd->type=1;
	else if(countofclusters<65525)/*fat16*/
		bd->type=2;
	else bd->type=3;/*fat32*/
	/*type have been determined*/
	printk("File System:Fat%d\n",fat_t[bd->type-1]);
	bd->resvdseccnt=bs->reserved;
	bd->sector_size=bytspersec;	
	bd->cluster_size=bs->cluster_size;	
	bd->fat_number=bs->fat_number;	
	bd->dir_number=rootentcnt;	
	bd->sector_number=totsec;			
	bd->sectorsperfat=fatsz;	
	bd->sectorspertrack=bs->sectorspertrack;	
	bd->heads=bs->heads;		
	bd->hidden=bs->hidden;
}

/*static struct inode * get_inode()
{
}*/
static int getfatentryval(struct super_block * bd,unsigned char * secbuf,int cluster,int offset)
{
	int val;
	if(bd->type==1)/*fat12*/
	{
		val=*((unsigned short *)&secbuf[offset]);
		if(cluster&0x0001)
			val=val>>4;/*ood*/
		else
			val=val&0xfff;/*even*/
	}
	else if(bd->type==2)/*fat16*/
		val=*((unsigned short *)&secbuf[offset]);
	else/*fat32*/
		val=(*((unsigned long *)&secbuf[offset]))&0x0fffffff;
	return val;
}	

/*if we know cluster number N,where is the entry for the cluster number*/
static int clustertofstsector(struct super_block * bd,int n)
{
	int i,offset;
	if(bd->type==1)
		offset=n+(n/2);
	else if(bd->type==2)
		offset=n*2;
	else 
		offset=n*4;
	i=bd->resvdseccnt+(offset/bd->sector_size);
	return i;
}
/*first sector of first root directory*/
static inline int firstrootdirsecnum(struct super_block * bd)
{
	return (bd->sectorsperfat)*(bd->fat_number)+bd->resvdseccnt+bd->base;
}
/*return root directory sectors count*/
static inline int rootdirsectors(struct super_block * bd)
{
	return (bd->dir_number*32+(bd->sector_size-1))/bd->sector_size;
}
/* get partition table in sector 0*/
static int get_part_table(struct partition_entry * table)
{
	unsigned char boot[512];
	ide_read(boot,512,0);
	if((boot[510]!=0x55)||(boot[511]!=0xaa))
		return 0;
	rep_movsb((char *)(boot+0x1be),table,sizeof(struct partition_entry)*4);
	return 1;
}
/*short name:8-char main part of the name,3-char extension*/
static void shortnamecopy(unsigned char * from,unsigned char * to)
{
	int i,end;
	for(i=7;i>=0;i--)
		if(*(from+i)!=' ')break;
	end=i;
	for(i=0;i<=end;i++)
	{
		*to=*(from+i);
		to++;
	}
	if(*(from+8)!=' ')
	{
		*to='.';
		to++;
	}
	for(i=8;i<11;i++)
	{
		if(*(from+i)!=' ')
		{
			*to=*(from+i);
			to++;
		}
	}
	*to=0;
}
int fat_read(int handle,void * buf,int size)
{
	return 1;
}
void fat_close(int handle)
{
}
int sys_open(char * name)
{
	int i;
	struct partition_entry table[4],*tap;
	struct dir_entry * dirp;
	unsigned char filename[13];
	/*get partition*/
	if(!get_part_table(table))
		return 0;
	printk("Partition:\n");
	for(i=0;i<4;i++)
	{
		tap=&table[i];
		if(tap->sysind!=NO_PART)
			printk("<hda%d%c[%d-%d]>\n",i,((tap->bootind==0x80)?'*':' '),tap->first_log,tap->first_log+tap->size-1);
	}
	/*read file*/
	if((table[0].sysind!=(FAT12_PART))&&(table[0].sysind!=(FAT32_PART)))
	{
		printk("file system(0x%x) not supported\n",table[0].sysind);
		return 0;
	}
	super_block.base=table[0].first_log;
	ide_read(data_buf,512,super_block.base);
	get_superblock(data_buf,&super_block);
	/*search path*/
	ide_read(data_buf,512,firstrootdirsecnum(&super_block));
	dirp=(struct dir_entry *)data_buf;
	filename[12]=0;/*for savety*/
	for(i=0;i<16;i++)
	{	
		if((dirp+i)->d.ldir.ord==0xE5)continue;
		if(((dirp+i)->d.ldir.attr&ATTR_LONG_NAME_MASK)!=ATTR_LONG_NAME)
		{
			shortnamecopy((dirp+i)->d.sdir.name,filename);
			printk("%s",filename);
			if(!strcmp(filename,name))printk("[*]");;
			if(((dirp+i)->d.sdir.attr&(ATTR_SUBDIR|ATTR_LABEL))==0x00)/*file*/
				printk(" ");
			else if(((dirp+i)->d.sdir.attr&(ATTR_SUBDIR|ATTR_LABEL))==ATTR_SUBDIR)/*dir*/
				printk("/ ");
			else if(((dirp+i)->d.sdir.attr&(ATTR_SUBDIR|ATTR_LABEL))==ATTR_LABEL)/*label*/
				printk("<LABLE> ");	
		}
		/*else found long name 
		{
		}*/		
	}
	return 1;
}

