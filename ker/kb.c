/*
 * written by cpuwolf
 * 2004/3
 */
 
#include <proto.h>
#include <wait.h>
#include <system.h>
#include <keymap.h>
#include <kb.h>
#include <section.h>

/*static char alt;*/
static char shift;
static char capslock;
/*static int tab;*/
static char scrolllock;
static char numlock;

struct kb_buf_t
{
	unsigned char * head;
	unsigned char * tail;
	int count;
	unsigned char buf[KB_BUF_SIZE];
};
struct kb_buf_t kb_line;/*the circular keyboard buffer*/

DECLARE_WAIT_QUEUE_HEAD(kb_wait_list);

extern void put_irq_handler(int,irq_handler);
void set_leds();
static int kbd_wait_for_input(void)
{
        int     n;
        int     status, data;
        n = TIMEOUT_CONST;
        do 
        {
                status = inb(KBD_STATUS_REG);
                if (!(status & KBD_OBF))continue;
		data = inb(KBD_DATA_REG);
                if (status & (KBD_GTO | KBD_PERR))continue;
		return (data & 0xff);
        } while (--n);
        return (-1);
}

static void kbd_write(int address, int data)
{
	int status;
	do 
	{
		status = inb(KBD_STATUS_REG);  
	} while (status & KBD_IBF);
	outb( address,data);
}

static void kb_wait()
{
		int i;
		for(i=0;i<TIMEOUT_CONST;i++)
				if((inb(KBD_STATUS_REG)&KBD_IBF)==0)
						break;
}

static void send_cmd(unsigned char c)
{
		kb_wait();
		outb(KBD_CNTL_REG,c);
}

static void status_process(unsigned char code)
{
	int status;
	status=((code&0x80)?0:1);/*press=1 release=0*/
	code&=(~0x80);
	switch(code)
	{
		case CAPSLOCK:/*caps lock*/
			capslock=1-capslock;
			set_leds();
			break;
		case NUMLOCK:/*num lock*/
			numlock=1-numlock;
			set_leds();
			break;
		case SCROLLLOCK:/*scroll lock*/
			scrolllock=1-scrolllock;
			set_leds();
			break;
		case 0x2a:/*l shift*/
			shift=status;
			break;
		case 0x36:/*r shift*/
			shift=status;
			break;
	}
}
/*keyboard interrupt*/	
int __ISR kbd_int(int irq)
{
		unsigned char code=0,km=0;
		unsigned char status;
		register struct kb_buf_t *kb;
		send_cmd(KBD_CNTL_DISABLE);
		status=inb(KBD_STATUS_REG);
		do
		{
			code=inb(KBD_DATA_REG);
			/*printk("kb:0x%x",code);*/
			status=inb(KBD_STATUS_REG);
		}while(status & KBD_OBF);
		kb=&kb_line;
		if(code&0x80)
		{
			km=code&(~0x80);
			if(km==CAPSLOCK||km==SCROLLLOCK||km==NUMLOCK)goto skip_key;
			/*skip release of CAPSLOCK,SCROLLLOCK,NUMLOCK*/
		}	
		if(kb->count<KB_BUF_SIZE)
		{
			*kb->head++=code;
			if(kb->head==kb->buf+KB_BUF_SIZE)kb->head=kb->buf;
			kb->count++;
		}/*else discard them*/
		wake_up(&kb_wait_list);
		skip_key:
		send_cmd(KBD_CNTL_ENABLE);
		return 1;
}
void set_leds()
{
		unsigned int leds;
		leds=(scrolllock)|(numlock<<1)|(capslock<<2);
		kbd_write(KBD_DATA_REG,LED_CODE);
		kbd_write(KBD_DATA_REG,leds);
}
/*read from keyboard*/
inline static unsigned int  map_key(scode) 
{
	if(shift^capslock)
		return keymap[scode][1];
	else
		return keymap[scode][0];
}

int kb_block_read(char * buf,int count)
{
	return 0;
}


int kb_read()
{
	register struct kb_buf_t *kb=&kb_line;
	unsigned int flags;
	unsigned char code=0;
	lock_irq_save(flags);
	while(!(kb->count>0))
		sleep_on(&kb_wait_list);
	if(kb->count>0)
	{
		code=*kb->tail++;
		if(kb->tail==kb->buf+KB_BUF_SIZE)kb->tail=kb->buf;
		kb->count--;
	}
	unlock_irq_restore(flags);
	status_process(code);
	/*key has been pressed*/
	if(!(code&0x80))
	{
		code=map_key(code&(~0x80));		
	}
	else code=0;
	return code;
}

void __init kb_init()
{
	register struct kb_buf_t *kb;
	kb=&kb_line;
	kb->head=kb->tail=kb->buf;
	kb->count=0;
	capslock=0;
	scrolllock=0;
	numlock=0;
	shift=0;
	put_irq_handler(1,kbd_int);
	/*while (kbd_wait_for_input() != -1)
	continue;*/
	printk("keyboard check\n");
	kbd_write(KBD_CNTL_REG, KBD_SELF_TEST);
	if (kbd_wait_for_input() != 0x55) 
	{
		printk("keyboard failed self test.");
	}
	/*printk("keyboard self test:OK\n");*/
	kbd_write(KBD_CNTL_REG, KBD_SELF_TEST2);
	if (kbd_wait_for_input() != 0x00) 
	{
		printk("keyboard failed self test 2.");
	}
	/*printk("keyboard self test 2:OK\n");*/
	kbd_write(KBD_CNTL_REG, KBD_CNTL_ENABLE);
	
	kbd_write(KBD_DATA_REG, KBD_RESET);
	if (kbd_wait_for_input() != KBD_ACK)
	{
		printk("reset kbd failed, no ACK.");
	}
	/*printk("reset kbd:OK\n");*/
	if (kbd_wait_for_input() != KBD_POR)
	{
		printk("reset kbd failed, not POR.");
	}
	/*printk("reset kbd:OK\n");*/
	kbd_write(KBD_DATA_REG, KBD_DISABLE);
	if (kbd_wait_for_input() != KBD_ACK) 
	{
		printk("disable kbd failed, no ACK.\n");
	}
	/*printk("disable kbd :OK\n");*/
	kbd_write(KBD_CNTL_REG, KBD_WRITE_MODE);
	kbd_write(KBD_DATA_REG, KBD_EKI|KBD_SYS|KBD_DMS|KBD_KCC);
	
	kbd_write(KBD_DATA_REG, KBD_ENABLE);
	if (kbd_wait_for_input() != KBD_ACK) 
	{
		printk("keyboard enable failed.\n");
	}
	/*printk("enable kbd :OK\n");*/
	enable_irq(1);
}
