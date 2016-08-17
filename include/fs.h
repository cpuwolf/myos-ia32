/*
 *	written by wei shuai
 *	2004/5/22
 *
 * */
 
#ifndef _OS_FS_H
#define _OS_FS_H

#include <type.h>
#include <slot.h>

/*hardware dependent struct*/
struct boot_sector
{
		u8_t jmp[3];		/*3*/
		u8_t oem[8];		/*11*/
		u8_t sector_size[2];	/*13*/
		u8_t cluster_size;	/*14*/
		u16_t reserved;	/*16*/
		u8_t fat_number;	/*17*/
		u8_t dir_number[2];	/*19*/
		u8_t totsec16[2];	/*21 fat16*/
		u8_t media;		/*22*/
		u16_t fatsz16;	/*24*/
		u16_t sectorspertrack;	/*26*/
		u16_t heads;		/*28*/
		u32_t hidden;		/*32*/
		u8_t totsec32[4];	/*36 fat32*/
		union
		{
			struct
			{
				u8_t drvnum;
				u8_t reserved;
				u8_t bootsig;
				u8_t volid[4];
				u8_t vollab[11];
				u8_t systype[8];
			} fat16;
			struct
			{
				u32_t fatsz32;
				u16_t extflags;
				u16_t fsver;
				u32_t rootclus;
				u16_t fsinfo;
				u16_t bkbootsec;
				u8_t reserved[12];
				u8_t drvnum;
				u8_t reserved1;
				u8_t bootsig;
				u8_t volid[4];
				u8_t vollab[11];
				u8_t systype[8];
			}fat32;
		}u;
		
};


struct dir_entry
{
	union
	{
		struct 
		{
			u8_t name[11];	/*11*/
			u8_t attr;	/*12*/
			u8_t ntres;	/*13*/
			u8_t crttimetenth;/*14*/
			u16_t crttime;	/*16*/
			u16_t crtdate;	/*18*/
			u16_t lstaccdate;/*20*/
			u16_t fstclushi;/*22*/
			u16_t wrttime;	/*24*/
			u16_t wrtdate;	/*26*/
			u16_t fstcluslo;/*28*/
			u32_t size;	/*32*/
		}s;
		struct
		{
			u8_t ord;/*1*/
			u8_t name1[10];/*11*/
			u8_t attr;/*12*/
			u8_t type;/*13*/
			u8_t chksum;/*14*/
			u8_t name2[12];/*26*/
			u16_t fstcluslo;/*28*/
			u8_t name3[4];/*32*/
		}l;
	}u;
};


#define 	ATTR_READONLY	0x01
#define 	ATTR_HIDDEN	0x02
#define 	ATTR_SYSTEM	0x04
#define 	ATTR_LABEL	0x08
#define 	ATTR_SUBDIR	0x10
#define 	ATTR_ARCHEVE	0x20
#define 	ATTR_LONG_NAME	(ATTR_READONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_LABEL)
#define 	ATTR_LONG_NAME_MASK	(ATTR_READONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_LABEL|ATTR_SUBDIR|ATTR_ARCHEVE)




/*hardware independent struct*/
struct super_block
{
		unsigned  base;/*where is the fat boot sector*/
		unsigned  sector_size;	
		unsigned  cluster_size;	
		unsigned  fat_number;		
		unsigned  sector_number;			
		unsigned  sectorsperfat;	
		unsigned  sectorspertrack;
		unsigned  resvdseccnt;
		unsigned  heads;		
		unsigned  hidden;
		unsigned  type;/*1 for fat12;2 for fat16;3 for fat32;*/	
		unsigned  dir_first_sec;
		unsigned  dir_number;	
		unsigned  data_first_sec;
		struct inode * rootdir;
};

struct inode
{
	struct super_block * i_sb;
	unsigned int size;			/*file size*/
	unsigned char name[255];
	unsigned int block[10];
	struct inode_operations * i_op;
	struct free_list free;
};
struct inode_operations
{
	int (*mkdir)();
};
struct dentry
{
	struct inode * d_inode;
	struct dentry_operations * d_op;
};
struct dentry_operations
{
	int (*scan)();
};
struct file
{
	struct file_operations * f_op;
	struct inode * f_ino;
	unsigned int f_pos;
	int count;	
	struct free_list free;
};

struct file_operations
{
	int (*open)(struct inode *,struct file *);
	int (*read)(struct file *,char *,int);
};


/*process open file*/
struct file_struct
{
	fd_set openfd;
	int maxopen;
	struct file * fd_array[5];
};
/*file system information*/
struct fs_struct
{
	struct inode * root;
};
#endif