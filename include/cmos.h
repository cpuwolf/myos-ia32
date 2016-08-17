#ifndef _OS_CMOS_H
#define _OS_CMOS_H

/*rtc*/
#define RTC_SECONDS		0
#define RTC_SECONDS_ALARM	1
#define RTC_MINUTES		2
#define RTC_MINUTES_ALARM	3
#define RTC_HOURS		4
#define RTC_HOURS_ALARM		5
#define RTC_DAY_OF_WEEK		6
#define RTC_DAY_OF_MONTH	7
#define RTC_MONTH		8
#define RTC_YEAR		9
#define RTC_CENTURY		0x32


#define CMOS_READ(addr) ({ \
outb(0x70,addr|0x80); \
inb(0x71); \
})

#define CMOS_WRITE(val, addr) ({ \
outb(0x70,addr|0x80); \
outb(val,0x71); \
})


#define BCD_TO_BIN(val) ((val)=((val)&15) + ((val)>>4)*10)


struct cmos_time {
	int sec;
	int min;
	int hour;
	int day;
	int mon;
	int year;
	int cen;
};

#endif