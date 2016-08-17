/* 
*	written by wei shuai
*	2004/9/26
*/

#include <partition.h>
#include <section.h>
#include <system.h>
#include <proto.h>


/* get partition table in sector 0*/
static int get_part_table(struct partition_entry * table)
{
	unsigned char boot[512];
	ide_read(boot,512,0);
	if((boot[510]!=0x55)||(boot[511]!=0xaa))
		return 0;
	rep_movsb((char *)(boot+0x1be),table,sizeof(struct partition_entry)*4);
	return 1;
}

void __init partition_check(struct partition_entry * tab)
{
	int i;
	struct partition_entry * tap;
	/*get partition*/
	if(!get_part_table(tab))
		return ;
	printk("Partition:\n");
	for(i=0;i<4;i++)
	{
		tap=&tab[i];
		if(tap->sysind!=NO_PART)
			printk("<hda%d%c[%d-%d]>\n",i,((tap->bootind==0x80)?'*':' '),tap->first_log,tap->first_log+tap->size-1);
	}
	/*read file*/
	if((tab[0].sysind!=(FAT12_PART))&&(tab[0].sysind!=(FAT32_PART)))
	{
		printk("file system(0x%x) not supported\n",tab[0].sysind);
		return ;
	}	
}