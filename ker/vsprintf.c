/* written by cpuwolf
 * 2004/3
 * */
#include <proto.h>
#include <proc.h>
#include <glob.h>
#include <system.h>
#include <stdarg.h>
#include <section.h>


inline static char atoi(int d)
{
		return d+48;
}
inline static char atoihex(int d)
{
	if(d<=9)return '0'+d;
	else return 'a'+d-10;
}
				
static char * int_to_str(char * s,unsigned int d)
{
	unsigned int y,i=0,j;
	char tmp;
	if(d==0)
	{
		s[0]='0';
		s[1]=0;
		s++;
		return s;	
	}
	else
	{
		while(d>=1)
		{
				y=d%10;
				d=d/10;
				s[i++]=atoi(y);
		}
		i--;
		for(j=0;j<=i/2;j++)
		{
				tmp=s[i-j];
				s[i-j]=s[j];
				s[j]=tmp;
		}
		s+=i;
		s++;
		return s;
	}
}

static char * int_to_hexstr(char * s,unsigned int d)
{
	unsigned int y,i=0,j;
	char tmp;
	if(d==0)
	{
		s[0]='0';
		s[1]=0;
		s++;
		return s;	
	}
	else
	{
		j=0;
		while(d>=1)
		{
				y=d&0xf;
				s[i++]=atoihex(y);
				d>>=4;
				j++;
		}
		i--;
		for(j=0;j<=i/2;j++)
		{
				tmp=s[i-j];
				s[i-j]=s[j];
				s[j]=tmp;
		}
		s+=i;
		s++;
		return s;
	}
}


/*used by user process and kernel */
void vsprintf(char * buf,const char * fmt, va_list args)
{
		char * str;
		unsigned char *tmp;
		for(str=buf;*fmt;++fmt)
		{
				if(*fmt!='%')
				{		
						*str=*fmt;
						str++;
						continue;
				}
				fmt++;
				switch(*fmt)
				{
				case 'c':	
						*str=(unsigned char)(va_arg(args,int)&0xff);
						str++;
						break;
				case 'd':
						str=int_to_str(str,va_arg(args,unsigned int));
						break;
				case 'x':
						str=int_to_hexstr(str,va_arg(args,unsigned int));
						break;
				case 's':
						for(tmp=va_arg(args,unsigned char *);*tmp;tmp++,str++)
						{
							*str=*tmp;		
						}
						break;
						
				//default:
				}
		}
		*str=0;
}

