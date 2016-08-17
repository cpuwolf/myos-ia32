/* written by cpuwolf
 * 2004/3
 * */
#include <proto.h>
#include <proc.h>
#include <glob.h>
#include <system.h>
#include <stdarg.h>
#include <section.h>

#define STACK_SIZE 100
#define SYS_COLOR  0x7
#define UDV_BASE 0x3d4


typedef struct console
{
	unsigned c_start;
	unsigned c_org;
	unsigned c_limit;
	unsigned c_cur;
	int c_row;
	int c_col;
}console_t;

static console_t cons;

void __init scr_init()
{
		cons.c_col=0;
		cons.c_row=0;
		cons.c_start=0xb8000;
		cons.c_org=cons.c_start;
}
void scroll_screen(console_t * con)
{
	memcpy((char *)(con->c_org+80*2),(char *)(con->c_org),80*24*2);
	memset((char *)(con->c_org+80*24*2),0,80*2);
}
void char_out(console_t * con,char c,unsigned char color)
{
		unsigned char buf[2];
		buf[0]=c;
		buf[1]=color;
        	memcpy(buf,(char *)(con->c_org+160*con->c_row+con->c_col*2),2);
}

static void set_cur(int cur)
{
	outb(UDV_BASE,14);
	outb(UDV_BASE+1,(cur>>8)&0xff);
	outb(UDV_BASE,15);
	outb(UDV_BASE+1,cur&0xff);
}


static void scroll_check()
{
	if(cons.c_col>=80)
	        {
	        	cons.c_row++;
	        	cons.c_col=0;
	        }
	        if(cons.c_row>=25)
		{
			scroll_screen(&cons);
			cons.c_row=24;
			cons.c_col=0;
		}
}
void cons_echo(char * s)
{
	unsigned flags;
	if(s==0)return;
	lock_irq_save(flags);
   	while(*s!=0)
        {
        	if(*s=='\n')
        	{
				cons.c_row++;
	        		cons.c_col=0;
	        		scroll_check();
	                	s++;
	                	continue;
	        }
                char_out(&cons,*s,SYS_COLOR);
                cons.c_cur=cons.c_row*80+cons.c_col;
		set_cur(cons.c_cur);
		cons.c_col++;
		scroll_check();
                s++;
        }
        unlock_irq_restore(flags);	
}

void tty_write(char * s)
{
	cons_echo(umap(current,s));
}


void printk(const char * fmt, ...)
{
		char stack[STACK_SIZE];/*alloc in stack*/
		va_list args;
		va_start(args,fmt);
		vsprintf(stack,fmt,args);
		va_end(args);
		cons_echo(stack);
}

void text_out(int row,int col,unsigned char color,const char * fmt,...)
{
	char buf[STACK_SIZE];
	char * s;
	va_list args;
	console_t textcons;
	textcons.c_col=col;
	textcons.c_row=row;
	textcons.c_start=0xb8000;
	textcons.c_org=textcons.c_start;
	va_start(args,fmt);
	vsprintf(buf,fmt,args);
	va_end(args);
	s=buf;
	while(*s!=0)
	{
		char_out(&textcons,*s,color);
		textcons.c_col++;
		s++;
	}
}
