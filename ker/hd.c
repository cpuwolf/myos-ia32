/*
 *
 * copyright(C)	wei shuai
 * 2004/4/9
 * 
 */
 
 /*	I/O register	*/
#include <type.h>
#include <const.h>
#include <system.h>
#include <wait.h>
#include <proc.h>
#include <proto.h>
#include <glob.h>
#include <section.h>
#include <hd.h>

static void do_request();

DECLARE_WAIT_QUEUE_HEAD(hd_wait_list);

struct command 
{
	u8_t command;
	u8_t device; /* device */
	u8_t count;
	u8_t sector;
	u16_t cylinder;
	u8_t comp;
	u8_t head; 
};


static struct ide_drive
{
	unsigned mainid;
	unsigned base;
	unsigned irq;
	unsigned state;
	unsigned pcylinders;
	unsigned pheads;
	unsigned psectors;
	unsigned lcylinders;
	unsigned lheads;
	unsigned lsectors;
	unsigned precomp;
	unsigned ctl;
	unsigned char max_multsect;
	unsigned buf_size;
	unsigned dword_io;
	unsigned lba;
	
}wini[2],*current_dev;

#define	READ 	2
#define WRITE 	3

#define BUF_SIZE 5
static struct request
{
	u32_t block;
	u32_t count;
	void * buf;
	struct ide_drive * dev;
	struct proc * waiter;
	unsigned type;
	unsigned mainid;
}req_buf[BUF_SIZE],*req_head,*req_tail;
static int req_count;

inline static int getstate(struct ide_drive *dev,unsigned char mask,unsigned char value)
{
	return ((in_byte(dev->base+REG_STATUS)&(mask))==(value));
}

int __ISR hd_handlerp(int irq)
{
	struct ide_drive *dev=current_dev;
	struct request *rq=req_head;
	int state;
	unsigned int flags;
	int retries=1000;
	do
	{
		state=in_byte(dev->base+REG_STATUS);
		/*printk("hd return status value:0x%x\n",state);*/
		if(state&STATUS_BUSY)
			continue;
		if(state&STATUS_DTRQ)
			goto ok_read;
	}while(--retries>0);
	printk("hd error code:0x%x\n",in_byte(dev->base+REG_ERROR));
	return 1;
ok_read:
	if(rq->type==READ)
	{
		lock_irq_save(flags);
		in_words(dev->base+REG_DATA,rq->buf,rq->count<<8);	
		wake_up_process(rq->waiter);
		req_count--;
		req_head++;
		if(req_head==(req_buf+BUF_SIZE))req_head=req_buf;
		do_request();
		unlock_irq_restore(flags);
	}
	return 1;	
}
int hd_handlers(int irq)
{
	return 1;
}
void hd_bh()
{
}
static int command(struct ide_drive * wn,struct command * cmd)
{
	if(!getstate(wn,STATUS_BUSY|STATUS_READY|STATUS_ERR,STATUS_READY))
	{
		printk("controller not ready!\n");
		return 0;
	}
	out_byte(wn->base+REG_CONTROL,wn->ctl);
	out_byte(wn->base+REG_LDH,0xa0|(cmd->device<<4)|cmd->head);/* select a device */
	out_byte(wn->base+REG_COUNT,cmd->count);
	out_byte(wn->base+REG_SECTOR,cmd->sector);
	out_byte(wn->base+REG_CYL_LOW,cmd->cylinder);
	out_byte(wn->base+REG_CYL_HIGH,cmd->cylinder>>8);
	out_byte(wn->base+REG_COMMAND,cmd->command);
	return 1;
}
static void do_readwrite(struct ide_drive * wn,struct request * rq)
{
	while(!getstate(wn,STATUS_BUSY|STATUS_READY|STATUS_ERR,STATUS_READY))printk("controller not ready!\n");
	/*printk("DUMP:disk:%d b:%d n:%d buf:%d\n",wn->mainid,rq->block,rq->count,(u32_t)rq->buf);*/
	out_byte(wn->base+REG_CONTROL,wn->ctl);
	
	out_byte(wn->base+REG_COUNT,rq->count);

	/*LBA*/
	out_byte(wn->base+REG_SECTOR,(rq->block)&0xff);
	out_byte(wn->base+REG_CYL_LOW,(rq->block>>8)&0xff);
	out_byte(wn->base+REG_CYL_HIGH,(rq->block>>16)&0xff);
	out_byte(wn->base+REG_LDH,0xa0|(wn->lba<<6)|(wn->mainid<<4)|((rq->block>>24)&0xf));

	/*count*/
	
	if(rq->type==READ)
		out_byte(wn->base+REG_COMMAND,CMD_MULTREAD);
	else if(rq->type==WRITE)
		out_byte(wn->base+REG_COMMAND,CMD_WRITE);
	
}
static int hd_specify(struct ide_drive * wn)
{
	int i,r;
	/*port check*/
	unsigned char dmabuf[512];
	unsigned char id_string[40];
	struct command cmd;
	r=in_byte(wn->base+REG_CYL_LOW);
	out_byte(wn->base+REG_CYL_LOW,~r);
	if(in_byte(wn->base+REG_CYL_LOW)==r)
	{
		printk("port check error!\n");
		return 0;
	}
	
	cmd.device=0;
	cmd.head=0;
	cmd.command=CMD_IDENTIFY;
	command(wn,&cmd);
	while(!getstate(wn,STATUS_DTRQ|STATUS_BUSY|STATUS_ERR,STATUS_DTRQ));
	in_words(wn->base+REG_DATA,dmabuf,256);
	for(i=0;i<40;i++)
	{
		id_string[i]=(((struct deviceid *)dmabuf)->model)[i^1];
		if(id_string[i]==' ')id_string[i]=0;
	}
	id_string[39]=0;
	wn->pcylinders=((struct deviceid *)dmabuf)->cylinders;
	wn->pheads=((struct deviceid *)dmabuf)->heads;
	wn->psectors=((struct deviceid *)dmabuf)->sectors;
	wn->lcylinders=((struct deviceid *)dmabuf)->lcylinders;
	wn->lheads=((struct deviceid *)dmabuf)->lheads;
	wn->lsectors=((struct deviceid *)dmabuf)->lsectors;
	wn->max_multsect=((struct deviceid *)dmabuf)->max_multsect;
	wn->dword_io=((struct deviceid *)dmabuf)->dword_io;
	wn->buf_size=((struct deviceid *)dmabuf)->buf_size;
	printk("ide0:%s DWordIO:%d bufsize:%d",id_string,wn->dword_io,wn->buf_size);
	if((((struct deviceid *)dmabuf)->method)&0x0200)
	{
		wn->lba=1;
		printk("[LBA]\n");
	}else wn->lba=0;
	printk("cy:%d head:%d sec:%d mult_sect:%d\n",wn->pcylinders,wn->pheads,wn->psectors,wn->max_multsect);
	printk("lcy:%d lhead:%d lsec:%d comp:%d\n",wn->lcylinders,wn->lheads,wn->lsectors,wn->precomp);
	
	return 1;
}
void __init hd_init()
{
	register struct ide_drive * wn;
	char numberdevice=0;
	/*u16_t vector[2];*/
	u8_t BIOS[16];
	current_dev=&wini[0];
	req_head=req_tail=req_buf;
	req_count=0;
	wn=&wini[0];
	wn->irq=14;
	wn->base=REG_BASE0;
	wn=&wini[1];
	wn->irq=15;
	wn->base=REG_BASE1;
	rep_movsb((void *)0x475,&numberdevice,1);
	rep_movsb((void *)(0x41*4),BIOS,16);
	wn=&wini[0];
	wn->lcylinders=*(u16_t *)BIOS;
	wn->lheads=*(BIOS+2);
	wn->precomp=*(u16_t *)(BIOS+5);
	wn->lsectors=*(BIOS+14);
	wn->ctl=0x08;
	wn->mainid=0;

	put_irq_handler(wini[0].irq,hd_handlerp);
	put_irq_handler(wini[1].irq,hd_handlers);
	enable_irq(wini[0].irq);
	/*enable_irq(wini[1].irq);	*/
	hd_specify(&wini[0]);
}
static void do_request(void)
{
	if(req_count>0)
	{
		do_readwrite(req_head->dev,req_head);
	}else return;
}

static int do_read(unsigned id,unsigned char * buf,int size,int block)
{
	unsigned int flags;
	if(req_count>=BUF_SIZE)return 0;
	req_tail->block=block;
	req_tail->count=(size>>9);
	req_tail->buf=buf;
	req_tail->dev=&wini[0];
	req_tail->type=READ;
	req_tail->waiter=current;
	req_tail++;
	lock_irq_save(flags);
	req_count++;
	if(req_tail==(req_buf+BUF_SIZE))req_tail=req_buf;
	if(req_count==1)do_request();
	sleep_on(&hd_wait_list);
	unlock_irq_restore(flags);
	return 1;
}
/*interface function*/
int ide_read(void *buf,int size,int block)
{
	if(buf==NULL)return 0;
	do_read(0,buf,size,block);
	return 0;
}
