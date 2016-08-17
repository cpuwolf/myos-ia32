/* 
*	written by wei shuai
*	2004/5/23
*/
#include <type.h>
#include <proto.h>
#include <system.h>
#include <fs.h>
#include <glob.h>
#include <proc.h>
#include <hd.h>
#include <string.h>
#include <buf.h>

void fs_init()
{
	struct ide_drive *wn=&wini[0];
	wn->part_ops(wn->table);
	filp_init();
	inode_init();
	mount_fs(wn);
	proc_fd_init(current);
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

static int get_unused_fd(struct proc * p)
{
	int fd;
	struct file_struct * files=&(p->files);
	fd=find_zero_bit(&(files->openfd));
	if(fd>=files->maxopen)
		return -1;
	set_bit(&(files->openfd),fd);
	return fd;
}

/*hardware dependent*/
struct inode * scan_dentry_buf(char * buf,int count,unsigned char * name)
{
	int i;
	struct dir_entry * de=(struct dir_entry *)buf;
	struct inode * rip;
	unsigned char filename[13];
	u16_t low,high;
	filename[12]=0;/*for savety*/
	for(i=0;i<(count>>5);i++)
	{	
		if((((de+i)->u.l.attr&ATTR_LONG_NAME_MASK)!=ATTR_LONG_NAME)&&((de+i)->u.l.ord!=0xE5))
		{
			shortnamecopy((de+i)->u.s.name,filename);
			printk("%s",filename);
			if(!strcmp(filename,name))
			{	
				rip=get_inode();
				if(!rip)
					return NULL;
				low=(de+i)->u.s.fstcluslo&0xFFFF;
				high=(de+i)->u.s.fstclushi&0xFFFF;
				rip->block[0]=(high<<16)|low;
				rip->size=(de+i)->u.s.size;
				strcpy(rip->name,filename);
				return rip;
			}
			if(((de+i)->u.s.attr&(ATTR_SUBDIR|ATTR_LABEL))==0x00)/*file*/
				printk("\n");
			else if(((de+i)->u.s.attr&(ATTR_SUBDIR|ATTR_LABEL))==ATTR_SUBDIR)/*dir*/
				printk("/\n");
			else if(((de+i)->u.s.attr&(ATTR_SUBDIR|ATTR_LABEL))==ATTR_LABEL)/*label*/
				printk("<LABLE>\n");	
		}
		/*else found long name 
		{
		}*/		
	}
	
	return NULL;
}

/*copy from user to kernel*/
static char *get_name(char * name)
{
	unsigned char * _name;
	_name=(unsigned char *)kmalloc(255);
	strcpy(_name,umap(current,name));
	return _name;
}
/*release name space*/
static void put_name(char * name)
{
	kfree(name);
}

inline static unsigned int FstSecofDataClu(struct super_block * bd,unsigned int n)
{
	return ((n-2)*bd->cluster_size)+bd->data_first_sec;
}
/*translate file logic sector to physics sector*/
inline static unsigned int LSecToPSec(struct inode * ino,unsigned int n,unsigned clucnt)
{	
	return FstSecofDataClu(ino->i_sb,ino->block[n/clucnt])+n%clucnt;
}
int sys_read(void *buf,int size,int count,int fp)
{
	char * kbuf;
	struct buf * bf;
	int LSec,secsz,SecCnt,LoadCnt,offset,copy,left;
	struct file * f;
	kbuf=umap(current,buf);
	if(kbuf==NULL)
		return 0;
	left=size*count;
	f=current->files.fd_array[fp];
	if(f==NULL)
		return 0;
	secsz=f->f_ino->i_sb->sector_size;
	/*normal=512*/
	LSec=f->f_pos/secsz;/*get first logic sector*/
	SecCnt=do_cdiv(left,secsz);/*how many sectors user want to read*/
	LoadCnt=do_cdiv(SecCnt,(BLOCK_SIZE/secsz));/*actural read times*/
	while(LoadCnt>0)
	{
		bf=get_block(LSecToPSec(f->f_ino,LSec,f->f_ino->i_sb->cluster_size));
		if(bf==NULL)
			return 0;
		offset=f->f_pos%secsz;
		copy=BLOCK_SIZE-offset;
		if(left-copy<0)
			copy=left;/*adjust*/
		(f->f_pos)+=copy;
		LSec-=BLOCK_SIZE/secsz;/*how many sectors have been read?*/
		LoadCnt--;
		memcpy(bf->data+offset,kbuf,copy);
		put_block(bf);
	}
}

int sys_open(char * name)
{
	int fd;
	char * tmp;
	struct buf * bp;
	struct inode * rip,*dip;
	struct file * f;
	rip=current->fs.root;
	tmp=get_name(name);
	bp=get_block(rip->block[0]);
	dip=scan_dentry_buf(bp->data,BLOCK_SIZE,tmp);
	if(dip==NULL)
		return -1;
	dip->i_sb=rip->i_sb;
	/*printk("\nfound %s--%dBytes\n",dip->name,dip->size);*/
	put_name(tmp);
	put_block(bp);
	read_map(dip);
	fd=get_unused_fd(current);
	if(fd>=0)
	{
		f=get_filp();
		if(f==NULL)
			return -1;
		f->f_pos=0;
		f->count=0;
		f->count++;
		f->f_ino=dip;
		current->files.fd_array[fd]=f;
	}
	return fd;
}

void proc_fd_init(struct proc * p)
{
	p->files.openfd=0;
	p->files.maxopen=5;
	p->fs.root=super_block.rootdir;
}

