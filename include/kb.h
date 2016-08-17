#ifndef _OS_KB_H
#define _OS_KB_H

/* keyboard io port */
#define KBD_STATUS_REG      0x64
#define KBD_CNTL_REG        0x64
#define KBD_DATA_REG	    0x60
/*
 * controller commands
 */
#define KBD_READ_MODE	    0x20
#define KBD_WRITE_MODE	    0x60
#define KBD_SELF_TEST	    0xAA
#define KBD_SELF_TEST2	    0xAB
#define KBD_CNTL_ENABLE	    0xAE
#define KBD_CNTL_DISABLE    0xAD

#define LED_CODE 		0xED
/*
 * keyboard commands
 */
#define KBD_ENABLE	    0xF4
#define KBD_DISABLE	    0xF5
#define KBD_RESET	    0xFF
/*
 * keyboard replies
 */
#define KBD_ACK		    0xFA
#define KBD_POR		    0xAA
/*
 * status register bits
 */
/* 
|7|6|5|4|3|2|1|0|
 | | | | | | | |
 | | | | | | |  -KBD_OBF
 | | | | | |  -KBD_IBF
 | | | | |  -KBD_SELFTEST
 | | | |  -
 | | |  -
 | |  -KBD_GTO
 |  -
  -KBD_PERR
*/  
  
#define KBD_OBF			0x01
#define KBD_IBF			0x02
#define KBD_SELFTEST		0x04
#define KBD_GTO		    	0x40
#define KBD_PERR	    	0x80
/*
 * keyboard controller mode register bits
 */
#define KBD_EKI		    0x01
#define KBD_SYS		    0x04
#define KBD_DMS		    0x20
#define KBD_KCC		   0x40

#define TIMEOUT_CONST	500000

#define KB_BUF_SIZE 32
#define NUMLOCK 	0x45
#define SCROLLLOCK 	0x46
#define CAPSLOCK	0x3A

#endif