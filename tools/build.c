/*
 * written by cpuwolf
 * 
 *	2004/9 
 */
#include <stdio.h>	/* fprintf */
#include <stdlib.h>	/* contains exit */
#include <sys/types.h>	/* unistd.h needs this */
#include <unistd.h>	/* contains read/write */
#include <fcntl.h>


void die(char * str)
{
	fprintf(stderr,"%s\n",str);
	exit(1);
}

void usage(void)
{
	die("Usage: build boot system [> image]");
}

int main(int argc, char ** argv)
{
	int i,c,id;
	char buf[1024];

	if (argc != 4)
		usage();
	for (i=0;i<sizeof buf; i++) buf[i]=0;
	if ((id=open(argv[1],O_RDONLY,0))<0)
		die("Unable to open 'boot'");

	i=read(id,buf,sizeof buf);
	fprintf(stderr,"Boot sector %d bytes.\n",i);
	buf[510]=0x55;
	buf[511]=0xAA;
	i=write(1,buf,512);
	if (i!=512)
		die("Write call failed");
	close (id);
	
/*open setup*/	
	if ((id=open(argv[2],O_RDONLY,0))<0)
		die("Unable to open 'setup'");

	for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	close(id);
	fprintf(stderr,"setup %d bytes.\n",i);
	
/*open system*/
	if ((id=open(argv[3],O_RDONLY,0))<0)
		die("Unable to open 'system'");
	
	for (i=0 ; (c=read(id,buf,sizeof buf))>0 ; i+=c )
		if (write(1,buf,c)!=c)
			die("Write call failed");
	close(id);
	fprintf(stderr,"system %d bytes.\n",i);
	return(0);
}
