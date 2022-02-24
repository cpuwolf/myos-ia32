/*
*	copyright(c)
* 	wei shuai	2003/4

* update below map on 2022 Feb. 25

|--------------------| 0x07C00
|       MBR          |
|--------------------|
|--------------------| 0x10000
|       kernel       |
|--------------------| 0x90000
|     bootsector     |
|--------------------| 0x90200
|       setup        |
|--------------------| 0x94000
|       stack        |
|--------------------| 0x98000

disassemble bootsect binary file:

> objdump -mi8086 -b binary -D boot/bootsect

*/

#include <config.h>


BOOTSEG=0x07C0
INITSEG=DEF_INITSEG
SETUPSEG=DEF_SETUPSEG
SYSSEG=DEF_SYSSEG

#define	SETUPSECTS	1

.globl _start
.code16
.text
_start:
	movw	$BOOTSEG, %ax	/*move itself to 0x9000*/
	movw	%ax, %ds
	movw	$INITSEG, %ax
	movw	%ax, %es
	movw	$256, %cx
	subw	%si, %si
	subw	%di, %di
	cld
	rep
	movsw
	ljmp	$INITSEG,$go

/*NOW we are in  INITSEG*/

go:	movw	$0xA000, %sp		/*setup real-mode stack*/
	movw	%ax, %ds
	movw	%ax, %ss

	movw	$0x0600,%ax
	movw	$0x0000,%cx
	movw	$0x184f,%dx
	movb	$7,%bh
	int 	$0x10		/*flush screen*/

	movb 	$0x02,%ah 	/*set cursor at 0:0*/
	xorw	%dx,%dx
	xorw	%bx,%bx
	int	$0x10

/*print 'The system is loading'*/
	lea	msg,%bp
	movw	$21,%cx
	movw	$0x0007,%bx
	movw	$0x1301,%ax
	int	$0x10


/*get floppy parameters(max sectors)*/
	xorw	%dx,%dx			#floppy disk 0
	movw	$0x0201,%ax
	movw	$0x0012,%cx		#try sector 18
	movw	$0x200+SETUPSECTS*200,%bx
	int 	$0x13
	jnc		got_sectors

	movb	$0x0f,%cl		#try sector	15
	movw	$0x0201,%ax
	int	$0x13
	jnc		got_sectors

/* read from HDD/USB-HDD */
	movb	$0x80,%dl		#hard disk 0
	jmp		load_next
got_sectors:
	movw	%cx,msectors

load_next:
/*now ! we load setup(0head,0track,2sect) */
	movw	$SETUPSEG,%ax
	movw	%ax,%es
	movw	%ax, Segment
	xorw    %bx,%bx
	movw	%bx, Offset
	movw	$SETUPSECTS,nsectors
	movw	$SETUPSECTS,Sectors
	movw	$0,head
	movw	$0,track
	movw	$2,sector
	movw	$(2-1),LBA
	call	read_nsectors

/*now! we load system(just follow the setup)*/
	movw    $SYSSEG,%ax
    movw    %ax,%es
	movw	%ax, Segment
    xorw    %bx,%bx
	movw	%bx, Offset
	movw    $50,nsectors
	movw    $50,Sectors
    movw    $0,head
    movw    $0,track
    movw    $(SETUPSECTS+2),sector
	movw    $(SETUPSECTS+2-1),LBA
    call    read_nsectors

	call	kill_motor
/*check setup*/
	movw $(508+512),%si
	cmpw	$SIG1,(%si)
	jne		gone_bad_setup

/*After loaded setup&system ,we jump to the setup*/

	ljmp	$SETUPSEG,$0

track:		.word 	0
head:		.word 	0
sector:		.word	0
nsectors:   .word   0   /*number of sectors to read*/
msectors:	.word 	0	/*max sectors of the floppy device*/

DiskAddressPacket:
   Size:	.byte 0x10
			.byte 0x00
   Sectors:	.word 0x01
   Offset:	.word 0x00
   Segment:	.word 0x1000
   LBA:		.quad 0x01


read_nsectors:#(read (track,head,sector) nsectors to es:bx)
read_lba:
	pushw	%ax
	pushw	%bx
/* check int #13 extension functions */
	movb	$0x41,%ah
	movw	$0x55AA,%bx
	int	$0x13
	popw	%bx
	popw	%ax
	#jc	ext_not_support

	movb	$0x42, %ah
	lea	DiskAddressPacket,%si
	int		$0x13
	jc	ext_not_support
	ret
ext_not_support:
	#sub	$2, %sp #fake news: int32 will sp +2
	pushw	%ax
	pushw	%bx
	movw 	$0x0e58,%ax
	movw	$0x0007,%bx
	int	$0x10			#print 'x'   0x58='x'
	popw	%bx
	popw	%ax

rep_read:
	call	read_sector
	addw	$512,%bx
	decw	nsectors

	cmpw    $0,nsectors
    jle	read_done

	incw	sector

	movw    sector,%ax
    cmpw    msectors,%ax
	jle	next_read			#have not read a track

	movw    $1,sector			#reset sector=1
	movw	$1,%ax				#have read a track
	sub		head,%ax
	jne	ok4_read				#head have not been Max
	incw	track				#head have been Max
ok4_read:
	movw	%ax,head
next_read:						#have not read a track
	jmp	rep_read

read_done:
	ret

read_sector:#(read (track,head,sector) to es:bx)
	pushw	%ax
	pushw	%bx
	movw 	$0x0e2e,%ax
	movw	$0x0007,%bx
	int	$0x10			#print '.'   0x2e='.'
	popw	%bx
	popw	%ax

	movw	track,%dx
	movw	sector,%cx	#sector
	movb	%dl,%ch		#track
	movw	head,%dx
	movb	%dl,%dh		#head
	xorb	%dl,%dl		#device 0
	movb	$0x02,%ah
	movb	$0x01,%al   #read one sector
	int 	$0x13

	jnc	sector_read_ok

	xorb	%dl,%dl
	xorw	%ax,%ax
	int 	$0x13		#reset fd
	pushw   %ax
    pushw   %bx
    movw    $0x0e2a,%ax
    movw    $0x0007,%bx
    int $0x10           #print '*'   0x2a='*'
    popw    %bx
    popw    %ax
	jmp	read_sector
sector_read_ok:
	ret

kill_motor:
	movw	$0x3f2,%dx
	xorb	%al,%al
	outb	%al,%dx
	ret

gone_bad_setup:
	pushw   %ax
    pushw   %bx
    movw    $0x0e44,%ax
    movw    $0x0007,%bx
    int $0x10           #print '*'   0x44='D'
    popw    %bx
    popw    %ax
dead:
	hlt
	jmp	dead
msg:	.ascii	"The system is loading"
		.byte	0x0d,0x0a

.org 510
	.word 	0xAA55
