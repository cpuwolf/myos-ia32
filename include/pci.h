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

#endif