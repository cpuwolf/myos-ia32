/*
 * written by cpuwolf
 * 2004/3
 */
 
#include <type.h>
#include <proto.h>
#include <const.h>
#include <system.h>
#include <section.h>
#include <pci.h>

struct interval
{
	u32_t base;
	u32_t size;
	u8_t type;
};

static struct pci_dev
{
	unsigned char bus;
	unsigned char device;
	unsigned char func;
	u16_t deviceid;
	u16_t vendor;
	u32_t class;
	u8_t header_type;
	u8_t irq;
	struct interval interval[6];
}pcidev[8];

static struct pci_dev * devptr=pcidev;

/* PCI main bridge io operation */
static void pci_in_byte(struct pci_dev * dev,int regaddr,u8_t * value)
{
	out_dword(0xCF8,(0x80000000|(dev->bus<<16)|(dev->device<<11)|(dev->func<<8)|regaddr));
	*value=in_byte(0xCFC+(regaddr&3));
}

static void pci_in_word(struct pci_dev * dev,int regaddr,u16_t * value)
{
	out_dword(0xCF8,(0x80000000|(dev->bus<<16)|(dev->device<<11)|(dev->func<<8)|regaddr));
	*value=in_word(0xCFC+(regaddr&2));
}

static void pci_in_dword(struct pci_dev * dev,int regaddr,u32_t * value)
{
	out_dword(0xCF8,(0x80000000|(dev->bus<<16)|(dev->device<<11)|(dev->func<<8)|regaddr));
	*value=in_dword(0xCFC);
}
/*static void pci_out_byte(struct pci_dev * dev,int regaddr,u8_t value)
{
	out_dword(0xCF8,(0x80000000|(dev->bus<<16)|(dev->device<<11)|(dev->func<<8)|regaddr));
	out_byte(0xCFC+(regaddr&3),value);
}

static void pci_out_word(struct pci_dev * dev,int regaddr,u16_t value)
{
	out_dword(0xCF8,(0x80000000|(dev->bus<<16)|(dev->device<<11)|(dev->func<<8)|regaddr));
	out_word(0xCFC+(regaddr&2),value);
}*/

static void pci_out_dword(struct pci_dev * dev,int regaddr,u32_t value)
{
	out_dword(0xCF8,(0x80000000|(dev->bus<<16)|(dev->device<<11)|(dev->func<<8)|regaddr));
	out_dword(0xCFC,value);
}
/* the operations are hardware depends*/

/*check bus 0 scanning for a bridge */
static int pci_check_main()
{
	struct pci_dev dev;
	u16_t x,c;
	int i,j;
	dev.bus=0;
	for(i=0;i<32;i++)
		for(j=0;j<8;j++)
		{
			dev.device=i;
			dev.func=j;
			pci_in_word(&dev,PCI_CLASS,&c);
			pci_in_word(&dev,PCI_VENDOR_ID,&x);
			if((x==0x8086)||(x==0x0e11)||(c==0x0600)||(c==0x0300))
			{
				printk("PCI: Using configuration type 1\n");
				return 1;
			}
		}
	return 0;
		
}

static void pci_dev_setup_irq(struct pci_dev * dev)
{
	u8_t irq;
	pci_in_byte(dev,PCI_INT_PIN,&irq);
	if(irq)pci_in_byte(dev,PCI_INT_LINE,&irq);
		dev->irq=irq;
}

inline static u32_t pci_intervalsize(u32_t sz,u32_t mask)
{
	u32_t size=sz&mask;
	size=size&~(size-1);
	return size;
}

static void pci_dev_setup_interval(struct pci_dev * dev)
{
	int i;
	u32_t base,size,reg;
	for(i=0;i<6;i++)
	{
		reg=PCI_BASE0+i*4;
		pci_in_dword(dev,reg,&base);
		pci_out_dword(dev,reg,0xffffffff);
		pci_in_dword(dev,reg,&size);
		pci_out_dword(dev,reg,base);
		/*if(size==0x0 || size==0xffffffff) invalid interval
			continue;*/
		if((base&0x01)==0)/* memory interval*/
		{
			dev->interval[i].base=(base&(~0xf));
			dev->interval[i].size=pci_intervalsize(size,(~0xf));
		}
		else/* io interval */
		{
			dev->interval[i].base=(base&(~0x3));
			dev->interval[i].size=pci_intervalsize(size,0xfffc);
		}
		dev->interval[i].type=base&0x01;
	}
}


/* enum device on a bus */
static void pci_device_enum(struct pci_dev * tmpdev)
{
	u32_t id;
	u32_t class;
	u8_t type;
	/*
	check the tmp device by checking the vendorid
	*/
	pci_in_dword(tmpdev,PCI_VENDOR_ID,&id);
	if(id==0x00000000||id==0xffffffff)
		return ;
	/* else setup a pci device */
	devptr->bus=tmpdev->bus;
	devptr->device=tmpdev->device;
	devptr->func=tmpdev->func;
	devptr->vendor=id & 0xffff;
	devptr->deviceid=(id >> 16) & 0xffff;
	pci_in_dword(tmpdev,PCI_CLASS,&class);
	pci_in_byte(tmpdev,PCI_HEADER,&type);
	devptr->class=(class>>=8);
	devptr->header_type=type & 0x7f;
	printk("PCI device 0x%x:0x%x [b%d d%d f%d]%d\n",devptr->vendor,devptr->deviceid,devptr->bus,devptr->device,devptr->func,devptr->header_type);
	switch(devptr->header_type)
	{
		case PCI_DEVICE_NORMAL:
			pci_dev_setup_irq(devptr);
			pci_dev_setup_interval(devptr);
			break;
		case PCI_DEVICE_BRIDGE:
			break;
	}
	/*devptr++;*/
}


static void pci_scan_bus()
{
	struct pci_dev tmp;
	unsigned i,j;
	tmp.bus=0;
	for(i=0;i<32;i++)
		for(j=0;j<8;j++)
		{
			tmp.device=i;
			tmp.func=j;
			pci_device_enum(&tmp);
		}
}
void __init pci_init()
{
	if(pci_check_main())
	{
		pci_scan_bus();
	}
	else 
	{
		printk("PCI: not detected\n");
		return;
	}
}
