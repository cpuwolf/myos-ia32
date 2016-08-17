
OUTPUT_FORMAT("a.out-i386-linux")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
	/*. = 0x1;*/
  	.text : {
	*(.text)
	}
		
 	.data : {
	*(.data)
	*(.rodata)
	}
	
	
	.bss : {
	*(.bss)
	}
	
	/* Sections to be discarded */
  /DISCARD/ : {
	*(.comment)
	*(.note)
	}

}
