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


void fs_init()
{
	struct ide_drive *wn=&wini[0];
	wn->part_ops(wn->table);
	mount_fs(wn);
}
/*
static struct inode * get_inode(struct dir_entry * ent)
{
}*/
inline static int FAToffset(struct super_block * bd,int n)
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

static unsigned int FATEntryVal(struct super_block * bd,int cluster)
{
	int val,offset,sector;
	unsigned char * secbuf;
	offset=FAToffset(bd,cluster);
	secbuf=(unsigned char *)kmalloc(512*2);
	/*if we know cluster number N,where is the first sector*/
	sector=bd->resvdseccnt+(offset/bd->sector_size);
	ide_read(secbuf,512*2,sector);
	offset=offset%bd->sector_size;
	if(bd->type==1)/*fat12*/
	{
		val=*((unsigned short *)&secbuf[offset]);
		if(cluster&0x0001)
			val=val>>4;/*ood*/
		else
			val=val&0xFFF;/*even*/
	}
	else if(bd->type==2)/*fat16*/
	{
		val=*((unsigned short *)&secbuf[offset]);
	}
	else/*fat32*/
	{
		val=(*((unsigned long *)&secbuf[offset]))&0x0FFFFFFF;
	}
	kfree(secbuf);
	return val;
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
/*
struct file * dentry_open(struct dentry * dentry)
{
	struct file * f=NULL;
	struct inode * inode;
	inode=dentry->d_inode;
	f->f_dentry=dentry;
	f->f_pos=0;
	f->count=0;
	f->f_op->open(inode,f);
}*/
/*hardware dependent*/
struct dentry * scan_dentry_buf(char * buf,int count,unsigned char * name)
{
	int i;
	struct dir_entry * de;
	/*struct dentry * dentry;*/
	unsigned char filename[13];
	de=(struct dir_entry *)buf;
	filename[12]=0;/*for savety*/
	for(i=0;i<(count>>5);i++)
	{	
		if((((de+i)->u.l.attr&ATTR_LONG_NAME_MASK)!=ATTR_LONG_NAME)&&((de+i)->u.l.ord!=0xE5))
		{
			shortnamecopy((de+i)->u.s.name,filename);
			printk("%s",filename);
			if(!strcmp(filename,name))
			{	
				printk("[*]");
				return NULL;
			}
			if(((de+i)->u.s.attr&(ATTR_SUBDIR|ATTR_LABEL))==0x00)/*file*/
				printk(" ");
			else if(((de+i)->u.s.attr&(ATTR_SUBDIR|ATTR_LABEL))==ATTR_SUBDIR)/*dir*/
				printk("/ ");
			else if(((de+i)->u.s.attr&(ATTR_SUBDIR|ATTR_LABEL))==ATTR_LABEL)/*label*/
				printk("<LABLE> ");	
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
	strcpy(_name,user_to_phys(current,name));
	return _name;
}
/*release name space*/
static void put_name(char * name)
{
	kfree(name);
}
int sys_open(char * name)
{
	int fd;
	char * tmp;
	unsigned char * data_buf;
	/*search path*/
	tmp=get_name(name);
	data_buf=(unsigned char *)kmalloc(512);
	ide_read(data_buf,512,super_block.dir_first_sec);
	scan_dentry_buf(data_buf,512,tmp);
	put_name(tmp);
	kfree(data_buf);
	fd=get_unused_fd(current);
	if(fd>=0);
	return 1;
}

