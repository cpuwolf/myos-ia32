/* 
*	written by cpuwolf
*	2004/2
*/
#include <proto.h>
#include <const.h>
#include <proc.h>
#include <system.h>
#include <section.h>

irq_handler irq_table[16];


void put_irq_handler(int irq,irq_handler handler)
{
	unsigned int flags;
	lock_irq_save(flags);
	irq_table[irq]=handler;
	unlock_irq_restore(flags);
}

int nothing_irq(int irq)
{
	printk("irq %d occur!\n",irq);
	return irq;
}


void enable_irq(int irq)
{
	unsigned int flags;
	lock_irq_save(flags);
	if(irq<8)
		imm_outb(INT_CTLMASK,imm_inb(INT_CTLMASK)&~(1<<irq));
	else
		imm_outb(INT2_CTLMASK,imm_inb(INT2_CTLMASK)&~(1<<(irq%8)));
	unlock_irq_restore(flags);
}
void disable_irq(int irq)
{
	unsigned int flags;
	lock_irq_save(flags);
	if(irq<8)
		imm_outb(INT_CTLMASK,imm_inb(INT_CTLMASK)|(1<<irq));
	else
		imm_outb(INT2_CTLMASK,imm_inb(INT2_CTLMASK)|(1<<(irq)));
	unlock_irq_restore(flags);
}

void __init intr_init()
{
	int i;
	lock();
	imm_outb(INT_CTL,ICW1);
	imm_outb(INT_CTLMASK,IRQ0_VECTOR);
	imm_outb(INT_CTLMASK,(1<<CASCADE_IRQ));
	imm_outb(INT_CTLMASK,ICW4);
	imm_outb(INT_CTLMASK,0xfb);
	
	imm_outb(INT2_CTL,ICW1);
	imm_outb(INT2_CTLMASK,IRQ8_VECTOR);
	imm_outb(INT2_CTLMASK,CASCADE_IRQ);
	imm_outb(INT2_CTLMASK,ICW4);
	imm_outb(INT2_CTLMASK,0xff);	
	for(i=0;i<16;i++)
	{
		irq_table[i]=nothing_irq;
	}
}



void FASTCALL do_IRQ(int irq)
{
	if(irq<8)
	{
		imm_outb(INT_CTLMASK,(imm_inb(INT_CTLMASK)|(1<<irq)));
		imm_outb(INT_CTL,I8259_ENABLE);
		unlock();
		irq_table[irq](irq);
		lock();
		imm_outb(INT_CTLMASK,(imm_inb(INT_CTLMASK)&(~(1<<irq))));
	}
	else
	{
		imm_outb(INT2_CTLMASK,(imm_inb(INT2_CTLMASK)|(1<<(irq-8))));
		imm_outb(INT_CTL,I8259_ENABLE);
		imm_outb(INT2_CTL,I8259_ENABLE);
		unlock();
		irq_table[irq](irq);
		lock();
		imm_outb(INT2_CTLMASK,(imm_inb(INT2_CTLMASK)&(~(1<<(irq-8)))));
	}
}

#define BUILD_IRQ(irq)\
void hwint##irq##(); \
__asm__("\n.globl hwint"#irq"\n"\
		"hwint"#irq":\n\t"\
		"pushl 	$"#irq"\n\t"\
		"jmp 	save\n\t");

/*Interrupt Service Routing Entry*/
BUILD_IRQ(0);
BUILD_IRQ(1);
BUILD_IRQ(2);
BUILD_IRQ(3);
BUILD_IRQ(4);
BUILD_IRQ(5);
BUILD_IRQ(6);
BUILD_IRQ(7);

BUILD_IRQ(8);
BUILD_IRQ(9);
BUILD_IRQ(10);
BUILD_IRQ(11);
BUILD_IRQ(12);
BUILD_IRQ(13);
BUILD_IRQ(14);
BUILD_IRQ(15);


