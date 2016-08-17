/*
	written by wei shuai
 		2004/08/29
*/
OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(start32)
SECTIONS
{
	. = 0x10000;
  	.text : {
	*(.text)
	}
	/*int service routing*/
	.text.isr : {
	*(.text.isr)
	}
		
 	.data : {
	*(.data)
	}
	
	.rodata : {
	*(.rodata)
	}
	
	/*system init routing*/
	.text.init : { 
	*(.text.init) 
	}
	
	_bss_start = .;		/*bss*/
	.bss : {
	*(.bss)
	}
	_bss_end = .;
	
	/* Sections to be discarded */
  	/DISCARD/ : {
	*(.comment)
	*(.note)
	}
}
