#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "a.out.h"

int main()
{
	int fd;
	struct exec hdr;
	fd=open("hello",O_EXCL);
	if(fd==-1)
	{
		perror("file not found");
		return 1;
	}
	read(fd,&hdr,sizeof(struct exec));
	printf("entry:0x%x\n",hdr.a_entry); 
	printf("text size:0x%x\n",hdr.a_text);
	printf("data size:0x%x\n",hdr.a_data);
	printf("bss size:0x%x\n",hdr.a_bss);
	close(fd);
	return 0;
}
