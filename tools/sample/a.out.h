struct exec
{
	unsigned long a_magic;
	unsigned a_text;
	unsigned a_data;
	unsigned a_bss;
	unsigned a_syms;
	unsigned a_entry;
	unsigned a_trsize;
	unsigned a_drsize;
};
