#ifndef _OS_PCI_H
#define _OS_PCI_H

/* config register  const */
#define PCI_VENDOR_ID 	0x00
#define PCI_DEVICE_ID 	0x02
#define PCI_COMMAND 	0x04
#define PCI_STATUS 	0x06
#define PCI_CLASS 	0x08
#define PCI_HEADER	0x0e
#define PCI_BASE0	0x10
/* type detail */
#define PCI_DEVICE_NORMAL 0
#define PCI_DEVICE_BRIDGE 1
#define PCI_DEVICE_CARBUS 2
#define PCI_INT_LINE	0x3c	
#define PCI_INT_PIN	0x3d
/*class*/
#define PCI_BASE_CLASS_BRIDGE		0x06
#define PCI_CLASS_BRIDGE_HOST		0x0600
#define PCI_CLASS_BRIDGE_ISA		0x0601
#define PCI_CLASS_BRIDGE_EISA		0x0602
#define PCI_CLASS_BRIDGE_MC		0x0603
#define PCI_CLASS_BRIDGE_PCI		0x0604
#define PCI_CLASS_BRIDGE_PCMCIA		0x0605
#define PCI_CLASS_BRIDGE_NUBUS		0x0606
#define PCI_CLASS_BRIDGE_CARDBUS	0x0607
#define PCI_CLASS_BRIDGE_RACEWAY	0x0608
#define PCI_CLASS_BRIDGE_OTHER		0x0680
#endif