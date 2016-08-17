/* 
*	written by cpuwolf
*	2004/2
*/

#include <proto.h>
#include <system.h>
#include <section.h>
#include <cmos.h>
#include <time.h>

extern void text_out(int row,int col,unsigned char color,const char * fmt,...);

static unsigned long clocktick; /*clock tick counter*/

static struct cmos_time clocktime;

/*hardware beep()*/
void beep()
{
	out_byte(0x61,(in_byte(0x61)|3));
	out_byte(TIMER_MODE,0xB6);/*counter2,lsb,msb,mode3,binary*/
	out_byte(TIMER2,(unsigned char)BEEP_FREQ);
	out_byte(TIMER2,(BEEP_FREQ>>8));
}
/*hardware unbeep()*/
void unbeep()
{
	out_byte(0x61,in_byte(0x61)|~3);
}

int __ISR clock_handler(int irq)
{
	lock();
	(*(unsigned long *)&clocktick)++;
	bill_process_time();
	unlock();
	text_out(0,40,0x1e,"  T:%d s %d:%d:%d",clocktick,clocktime.hour,clocktime.min,clocktime.sec);
	text_out(1,40,0x1e,"  %d0%d/%d/%d",clocktime.cen,clocktime.year,clocktime.mon,clocktime.day);
	return 1;
}
static inline void get_cmos_time()
{
	clocktime.sec = CMOS_READ(RTC_SECONDS);
	clocktime.min = CMOS_READ(RTC_MINUTES);
	clocktime.hour = CMOS_READ(RTC_HOURS);
	clocktime.day = CMOS_READ(RTC_DAY_OF_MONTH);
	clocktime.mon = CMOS_READ(RTC_MONTH);
	clocktime.year = CMOS_READ(RTC_YEAR);
	clocktime.cen = CMOS_READ(RTC_CENTURY);
	BCD_TO_BIN(clocktime.sec);
	BCD_TO_BIN(clocktime.min);
	BCD_TO_BIN(clocktime.hour);
	BCD_TO_BIN(clocktime.day);
	BCD_TO_BIN(clocktime.mon);
	BCD_TO_BIN(clocktime.year);
	BCD_TO_BIN(clocktime.cen);	
}
void __init clock_init()
{
	out_byte(TIMER_MODE,SQUARE_WAVE);
	out_byte(TIMER0,(unsigned char)TIMER_COUNT);
	out_byte(TIMER0,TIMER_COUNT>>8);
	clocktick=0;
	/*read CMOS time*/
	get_cmos_time();
	/*irq handler init*/
	put_irq_handler(0,clock_handler);
	enable_irq(0);
}

