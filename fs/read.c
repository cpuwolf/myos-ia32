/* 
*	written by wei shuai
*	2004/5/23
*/
#include <type.h>
#include <proto.h>
#include <system.h>
#include <fs.h>
#include <glob.h>
#include <buf.h>


static unsigned int FstSecofDataClu(struct super_block * bd,unsigned int n)
{
	return ((n-2)*bd->cluster_size)+bd->data_first_sec;
}


static void ClusterRead(char ** buf,struct inode * ino,int nr,int offset,int size)
{
	struct buf * bf;
	int Sec,clusz;
	if(nr>10 || nr<0)
	{
		printk("invalid cluster number%d\n",nr);
		return;
	}
	Sec=FstSecofDataClu(ino->i_sb,ino->block[nr]);
	clusz=ino->i_sb->sector_size*ino->i_sb->cluster_size;
	
	bf=get_block(Sec,clusz);
	if(bf==NULL)
		return;
	memcpy(bf->data+offset,*buf,size);
	*buf+=size;
	put_block(bf);
}

static int eofile(struct file * f)
{
	if(f->f_ino->size <= f->f_pos)
		return 1;
	return 0;
}
static int filp_read(char * buf,int size,struct file * f)
{
	int secsz,Clusz,filesz,Clu;
	int offset,left,valid,oldpos;
	
	left=size;
	oldpos=f->f_pos;
	secsz=f->f_ino->i_sb->sector_size;		/*normal=512(byte[s])*/
	Clusz=f->f_ino->i_sb->cluster_size*secsz;	/*Unit:(byte[s])*/
	filesz=f->f_ino->size;				/*file size*/
	
	while((left > 0)&&(filesz > f->f_pos))
	{
		Clu=f->f_pos/Clusz;			/*the cluster number of f_pos*/
		offset=f->f_pos%Clusz;			/*the offset of f_pos in current cluster*/
		
		if((Clu == filesz/Clusz)&&(filesz % Clusz))		/*final cluster*/
			valid=filesz % Clusz;
		else	
			valid=Clusz;			
		valid-=offset;				/*valid size to read*/
		
		if(left<valid)
			valid=left;
		
		ClusterRead(&buf,f->f_ino,Clu,offset,valid);	/*read a cluster*/
	
		f->f_pos+=valid;

		left-=valid;
	}
	return (f->f_pos-oldpos);
}

int sys_feof(int fp)
{
	struct file * f;
	f=current->files.fd_array[fp];
	if(f==NULL)
		return 0;
	return eofile(f);
}
int sys_read(void *buf,int size,int count,int fp)
{
	char * kbuf;
	struct file * f;
	int read;
	kbuf=umap(current,buf);
	if(kbuf==NULL)
		return 0;
	f=current->files.fd_array[fp];
	if(f==NULL)
		return 0;
	read=filp_read((char *)buf,size*count,f);
	return read/size;
}

