/*
 *
 * copyright(C)	wei shuai
 * 2004/4/9
 * 
 */
 
#ifndef _OS_PROTO_H
#define _OS_PROTO_H

#include <type.h>
#include <proc.h>
#include <wait.h>

#include <hd.h>
#include <partition.h>
#include <buf.h>

/* init hardware function*/
void intel_init();
void kb_init();
void clock_init();
void hd_init();
void pci_init();
void mem_init();
void prot_init();
void intr_init();/* i8259 init function*/
void proc_init();
/* kernel console output function*/
void scr_init();
void printk(const char *,...);

/*system.c*/
void in_words(unsigned short port,void * dest,unsigned bcount);

/* exception entry function*/
void divide_error();
void debug_exception();
void nmi();
void breakpoint_exception();
void overflow();
void bounds_check();
void inval_opcode();
void copr_not_avaiable();
void double_fault();
void copr_segment();
void general_protection();
void stack_exception();
void inval_tss();
void segment_not_present();	

/*irq.c*/
/* i8259 hardware control function*/
void enable_irq(int irq);
void disable_irq(int irq);
extern void put_irq_handler(int,irq_handler);
void hwint0();/*i8259 master */
void hwint1();
void hwint2();
void hwint3();
void hwint4();
void hwint5();
void hwint6();
void hwint7();
void hwint8();/* i8259 slave*/
void hwint9();
void hwint10();
void hwint11();
void hwint12();
void hwint13();
void hwint14();
void hwint15();

/*sys.S*/
extern void ret_with_schedule();
extern void system_call();/* system call*/

/*proc.c*/
extern void bill_process_time();
extern void RoundRobin();
extern void proc_ready(struct proc * rp);
extern void proc_block(struct proc * bp);
extern struct proc * pick_proc();
extern void * umap(struct proc * p,void * offset);

/*schedule.c*/
extern void schedule();
extern void sleep_on(wait_queue_head_t * head);
extern void wake_up_process(struct proc * p);
extern void wake_up(wait_queue_head_t * head);

/*mem.c*/
extern void Kmem_free(void *);
extern void * Kmem_alloc(unsigned int);
extern void mem_show();

extern void * kmalloc(unsigned int);
extern void kfree(void *);

/*hd.c*/
extern int ide_read(void *buf,int size,int block);

/*partition.c*/
extern void partition_check(struct partition_entry *);

/*super.c*/
extern void mount_fs(struct ide_drive *);

/*open.c*/
extern void fs_init();
extern void proc_fd_init(struct proc *);

/*inode.c*/
extern void inode_init();
extern struct inode * get_inode();
extern void put_inode(struct inode *);
extern void read_map(struct inode * i);

/*block.c*/
extern struct buf * get_block(int);
extern void put_block(struct buf * );

/*filedes.c*/
struct file * get_filp();
void put_filp(struct file *);
void filp_init();
#endif




