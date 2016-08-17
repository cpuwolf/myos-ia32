/*
 *	written by wei shuai
 *	2004/8/2
 *
 */
 
#ifndef _OS_PARTITION_H
#define _OS_PARTITION_H 

struct partition_entry
{
	unsigned char bootind;
	unsigned char start_head;
	unsigned char start_sec;
	unsigned char start_cyl;
	unsigned char sysind;
	unsigned char last_head;
	unsigned char last_sec;
	unsigned char last_cyl;
	unsigned int first_log;
	unsigned int size;
};

#define NO_PART 	0x00
#define FAT12_PART 	0x06
#define NTFS_PART	0x07
#define FAT32_PART 	0x0B
#define EXT_PART 	0x05

#endif