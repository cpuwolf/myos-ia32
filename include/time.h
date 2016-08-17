#ifndef _OS_TIME_H
#define _OS_TIME_H

#define TIMER_MODE 0x43
#define TIMER0 0x40
#define TIMER1 0x41
#define TIMER2 0x42
#define HZ	60
#define CLOCK_TICK_RATE	1193182 /* Underlying HZ */
#define TIMER_COUNT (CLOCK_TICK_RATE / HZ)
#define SQUARE_WAVE 0x34
/*beep*/
#define BEEP_FREQ	0x533

#endif