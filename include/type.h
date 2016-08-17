#ifndef _OS_TYPE_H
#define _OS_TYPE_H

typedef unsigned char u8_t;
typedef unsigned short u16_t;
typedef unsigned int u32_t;

typedef int (* irq_handler)(int);
typedef void (* task_ptr)(void);

#define NULL (void *)0

#endif