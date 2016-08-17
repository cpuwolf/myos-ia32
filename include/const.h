#ifndef _OS_CONST_H
#define _OS_CONST_H


/* i8259 interupt */
#define INT_CTL		0x20
#define INT_CTLMASK	0x21
#define INT2_CTL	0xa0
#define INT2_CTLMASK	0xa1

#define I8259_ENABLE	0x20

#define ICW1		0x11
#define ICW4		0x01

#define CASCADE_IRQ	2

#define IRQ0_VECTOR	0x28
#define IRQ8_VECTOR	0x30


#define CS_INDEX 	1
#define DS_INDEX 	2
#define GDT_INDEX 	3
#define IDT_INDEX 	4
#define TSS_INDEX	5
#define FIRST_LDT_INDEX	6

/*used by  kernel*/
#define CS_SELECTOR 	0x08
#define DS_SELECTOR 	0x10
#define GDT_SELECTOR 	0x18
#define IDT_SELECTOR 	0x20
#define TSS_SELECTOR	0x28

#define SYS_VECTOR 	0x3C

#define NR_TASK 	10
#define GDT_SIZE 	(FIRST_LDT_INDEX+NR_TASK)
#define IDT_SIZE 	70

/*user process kernel thread*/
#define FREE_STACK_SIZE (1024*3)
#define KERNEL_THREAD_SIZE (sizeof(struct stackframe)+FREE_STACK_SIZE)

#endif


