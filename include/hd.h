#ifndef _OS_HD_H
#define _OS_HD_H

#include <type.h>
#include <partition.h>

#define REG_BASE0	0X1F0
#define REG_BASE1	0X170

#define	REG_DATA	0

#define	REG_PRECOMP	1
#define REG_ERROR	REG_PRECOMP

#define	REG_COUNT	2
#define	REG_SECTOR	3
#define	REG_CYL_LOW	4
#define	REG_CYL_HIGH	5
#define	REG_LDH	6

#define	REG_STATUS	7
#define	REG_COMMAND	REG_STATUS

#define REG_CONTROL 	0x206
#define REG_ALTSTATUS	REG_CONTROL
/*	const		*/
#define	LDH_DEFAULT	0XA0
#define	LDH_LBA	0X40
/* 
|7|6|5|4|3|2|1|0|
 | | | | | | | |
 | | | | | | |  -STATUS_ERR
 | | | | | |  -STATUS_IDX
 | | | | |  -STATUS_CRD  
 | | | |  -STATUS_DTRQ
 | | |  -STATUS_SEEKC
 | |  -STATUS_WF
 |  -STATUS_READY
  -STATUS_BUSY
*/  
#define	STATUS_BUSY	0X80
#define	STATUS_READY	0X40
#define	STATUS_WF	0X20
#define	STATUS_SEEKC	0X10
#define	STATUS_DTRQ	0X08
#define	STATUS_CRD	0X04
#define	STATUS_IDX	0X02
#define	STATUS_ERR	0X01
/* command */
#define CMD_RESTORE	0x10
#define	CMD_READ	0X20
#define CMD_READTRY	0X21
#define CMD_WRITE	0X30
#define CMD_MULTREAD	0xc4
#define CMD_MULTWRITE	0xc5
#define CMD_FORMAT 	0x50
#define CMD_DIAGNOSE 	0x90
#define CMD_INIT	0X91
#define CMD_CALIBRATE	0X10
#define CMD_READBUF	0XE4
#define CMD_IDENTIFY 	0XEC
#define CMD_SEEK	0x70
#define	CMD_SETFEATURE 	0xef


struct deviceid
{
	u16_t	config;
	u16_t	cylinders;	/* "physical" cyls */
	u16_t	pres0;
	u16_t	heads;		/* "physical" heads */
	u16_t	track_bytes;	/* unformatted bytes per track */
	u16_t	sector_bytes;	/* unformatted bytes per sector */
	u16_t	sectors;		/* "physical" sectors per track */
	u16_t	vendor0;
	u16_t	vendor1;
	u16_t	vendor2;
	u8_t	sn[20];
	u16_t	controller_type;
	u16_t	buf_size;
	u16_t	ecc;
	u8_t	fw_rev[8];
	u8_t	model[40];
	u8_t	max_multsect;
	u8_t	vendor3;
	u16_t	dword_io;
	u8_t	vendor4;
	u8_t	method;		/* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup*/
	u16_t	presv50;
	u8_t	vendor5;
	u8_t	tPIO;
	u8_t	vendor6;
	u8_t	tDMA;
	u16_t	field;
	u16_t	lcylinders;	/* logical cylinders */
	u16_t	lheads;		/* logical heads */
	u16_t	lsectors;		/* logical sectors per track */
	u32_t	lcapacity;	/* logical total sectors on drive */
	u8_t	multisect;	/* current multiple sector count */
	u8_t	multsect_valid;
	u32_t	lba_capacity;	/* total number of sectors */
};

struct ide_drive
{
	unsigned mainid;
	unsigned base;
	unsigned irq;
	unsigned state;
	unsigned pcylinders;
	unsigned pheads;
	unsigned psectors;
	unsigned capacity;
	unsigned lcylinders;
	unsigned lheads;
	unsigned lsectors;
	unsigned precomp;
	unsigned ctl;
	unsigned char max_multsect;
	unsigned buf_size;
	unsigned dword_io;
	unsigned lba;
	struct partition_entry table[4];
	void (*part_ops)();
};

#endif