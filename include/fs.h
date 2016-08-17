/*
 *	written by wei shuai
 *	2004/5/22
 *
 * */
 
#ifndef _OS_FS_H
#define _OS_FS_H

struct boot_sector
{
		unsigned char jmp[3];		/*3*/
		unsigned char oem[8];		/*11*/
		unsigned char sector_size[2];	/*13*/
		unsigned char cluster_size;	/*14*/
		unsigned short reserved;	/*16*/
		unsigned char fat_number;	/*17*/
		unsigned char dir_number[2];	/*19*/
		unsigned char totsec16[2];	/*21 fat16*/
		unsigned char media;		/*22*/
		unsigned short fatsz16;	/*24*/
		unsigned short sectorspertrack;	/*26*/
		unsigned short heads;		/*28*/
		unsigned long hidden;		/*32*/
		unsigned char totsec32[4];	/*36 fat32*/
		union
		{
			struct
			{
				unsigned char drvnum;
				unsigned char reserved;
				unsigned char bootsig;
				unsigned char volid[4];
				unsigned char vollab[11];
				unsigned char systype[8];
			} fat16;
			struct
			{
				unsigned long fatsz32;
				unsigned short extflags;
				unsigned short fsver;
				unsigned long rootclus;
				unsigned short fsinfo;
				unsigned short bkbootsec;
				unsigned char reserved[12];
				unsigned char drvnum;
				unsigned char reserved1;
				unsigned char bootsig;
				unsigned char volid[4];
				unsigned char vollab[11];
				unsigned char systype[8];
			}fat32;
		}spec;
		
};
/*used for myself*/
struct super_block
{
		unsigned  base;/*where is the fat boot sector*/
		unsigned  sector_size;	
		unsigned  cluster_size;	
		unsigned  fat_number;	
		unsigned  dir_number;	
		unsigned  sector_number;			
		unsigned  sectorsperfat;	
		unsigned  sectorspertrack;
		unsigned  resvdseccnt;
		unsigned  heads;		
		unsigned  hidden;
		unsigned  type;/*1 for fat12;2 for fat16;3 for fat32;*/		
};
struct inode
{
	struct super_block * sb_p;
	unsigned int size;			/*file size*/
	unsigned char name[255];
	unsigned int block[6];
};
struct dir_entry
{
	union
	{
		struct 
		{
			unsigned char name[11];	/*11*/
			unsigned char attr;	/*12*/
			unsigned char ntres;	/*13*/
			unsigned char crttimetenth;/*14*/
			unsigned short crttime;	/*16*/
			unsigned short crtdate;	/*18*/
			unsigned short lstaccdate;/*20*/
			unsigned short fstclushi;/*22*/
			unsigned short wrttime;	/*24*/
			unsigned short wrtdate;	/*26*/
			unsigned short fstcluslo;/*28*/
			unsigned long size;	/*32*/
		}sdir;
		struct
		{
			unsigned char ord;/*1*/
			unsigned char name1[10];/*11*/
			unsigned char attr;/*12*/
			unsigned char type;/*13*/
			unsigned char chksum;/*14*/
			unsigned char name2[12];/*26*/
			unsigned short fstcluslo;/*28*/
			unsigned char name3[4];/*32*/
		}ldir;
	}d;
};


#define 	ATTR_READONLY	0x01
#define 	ATTR_HIDDEN	0x02
#define 	ATTR_SYSTEM	0x04
#define 	ATTR_LABEL	0x08
#define 	ATTR_SUBDIR	0x10
#define 	ATTR_ARCHEVE	0x20
#define 	ATTR_LONG_NAME	(ATTR_READONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_LABEL)
#define 	ATTR_LONG_NAME_MASK	(ATTR_READONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_LABEL|ATTR_SUBDIR|ATTR_ARCHEVE)

#endif