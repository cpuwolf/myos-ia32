/*
 *	written by wei shuai
 *	2004/5/22
 *
 * */
#ifndef _OS_ELF_H
#define _OS_ELF_H


#define EI_NIDENT 16

typedef struct{
	unsigned char e_ident[EI_NIDENT];
	unsigned short e_type ;
	unsigned short e_machine ;
	unsigned long e_version ;
	unsigned long e_entry ;
	unsigned long e_phoff;	/*the program header table¡¯s file offset*/
	unsigned long e_shoff;	/*the section header table¡¯s file offset*/
	unsigned long e_flags;	/*processor-specific flags*/
	unsigned short e_ehsize;	/*ELF header¡¯s size*/
	unsigned short e_phentsize ;
	unsigned short e_phnum;	/*the number of entries in the program header table.*/
	unsigned short e_shentsize; /*a section header¡¯s size*/
	unsigned short e_shnum ;/*the number of entries in the section header table*/
	unsigned short e_shstrndx ;
}Elf32_Ehdr;

/*e_ident bit field*/
#define EI_MAG0 0 /*File identification 0x7f*/
#define EI_MAG1 1 /*File identification 'E'*/
#define EI_MAG2 2 /*File identification 'L'*/
#define EI_MAG3 3 /*File identification 'F'*/
#define EI_CLASS 4 /*File class*/
	#define ELFCLASSNONE 0 /*Invalid class*/
	#define ELFCLASS32 1 /*32-bit objects*/
	#define ELFCLASS64 2 /*64-bit objects*/
#define EI_DATA 5 /*Data encoding*/
#define EI_VERSION 6 /*File version*/
#define EI_PAD 7 /*Start of padding bytes*/
#define EI_NIDENT 16 /*Size of e_ident[]*/

/*e_type*/
#define ET_NONE 0 /*No file type*/
#define ET_REL 1 /*Relocatable file*/
#define ET_EXEC 2 /*Executable file*/
#define ET_DYN 3 /*Shared object file*/
#define ET_CORE 4 /*Core file*/
#define ET_LOPROC 0xff00 /*Processor-specific*/
#define ET_HIPROC 0xffff /*Processor-specific*/

/*e_machine*/
#define EM_386 3 /*Intel 80386*/


/*e_version*/
#define EV_NONE 0 /*Invalid version*/
#define EV_CURRENT 1 /*Current version*/

typedef struct {
unsigned long sh_name;
unsigned long sh_type;
unsigned long sh_flags;
unsigned long sh_addr;/*the address at which the section¡¯s first byte should reside*/
unsigned long sh_offset;
unsigned long sh_size;/*the section¡¯s size*/
unsigned long sh_link;
unsigned long sh_info;
unsigned long sh_addralign ;
unsigned long sh_entsize;
}Elf32_Shdr;


/*sh_type*/
#define SHT_NULL 0
#define SHT_PROGBITS 1
#define SHT_SYMTAB 2
#define SHT_STRTAB 3
#define SHT_RELA 4
#define SHT_HASH 5
#define SHT_DYNAMIC 6
#define SHT_NOTE 7
#define SHT_NOBITS 8
#define SHT_REL 9
#define SHT_SHLIB 10
#define SHT_DYNSYM 11
#define SHT_LOPROC 0x70000000
#define SHT_HIPROC 0x7fffffff
#define SHT_LOUSER 0x80000000
#define SHT_HIUSER 0xffffffff
/*sh_flags*/
#define SHF_WRITE 0x1/*The section contains data that should be writable*/
#define SHF_ALLOC 0x2
#define SHF_EXECINSTR 0x4 /*The section contains executable machine instructions.*/
#define SHF_MASKPROC 0xf0000000

/*
.bss 		SHT_NOBITS 	SHF_ALLOC + SHF_WRITE
.comment 	SHT_PROGBITS 	none
.data 		SHT_PROGBITS 	SHF_ALLOC + SHF_WRITE
.data1 		SHT_PROGBITS 	SHF_ALLOC + SHF_WRITE
.debug 		SHT_PROGBITS 	none
.dynamic 	SHT_DYNAMIC 	see below
.dynstr 	SHT_STRTAB 	SHF_ALLOC
.dynsym 	SHT_DYNSYM 	SHF_ALLOC
.fini 		SHT_PROGBITS 	SHF_ALLOC + SHF_EXECINSTR
.got 		SHT_PROGBITS 	see below
.hash		SHT_HASH 	SHF_ALLOC
.init 		SHT_PROGBITS 	SHF_ALLOC + SHF_EXECINSTR
.interp 	SHT_PROGBITS 	see below
.line 		SHT_PROGBITS 	none
.note 		SHT_NOTE 	none
.plt 		SHT_PROGBITS 	see below
.relname 	SHT_REL 	see below
.relaname 	SHT_RELA 	see below
.rodata 	SHT_PROGBITS 	SHF_ALLOC
.rodata1 	SHT_PROGBITS 	SHF_ALLOC
.shstrtab 	SHT_STRTAB 	none
.strtab 	SHT_STRTAB 	see below
.symtab 	SHT_SYMTAB 	see below
.text 		SHT_PROGBITS 	SHF_ALLOC + SHF_EXECINSTR
*/

typedef struct{
unsigned long p_type;
unsigned long p_offset;
unsigned long p_vaddr;
unsigned long p_paddr;
unsigned long p_filesz;
unsigned long p_memsz;
unsigned long p_flags;
unsigned long p_align;
}Elf32_Phdr;

/*p_type*/
#define PT_NULL 0
#define PT_LOAD 1
#define PT_DYNAMIC 2
#define PT_INTERP 3
#define PT_NOTE 4
#define PT_SHLIB 5
#define PT_PHDR 6
#define PT_LOPROC 0x70000000
#define PT_HIPROC 0x7fffffff


#endif