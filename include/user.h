/* 
*	written by wei shuai
*	2004/8/28
*/

#include <stdarg.h>

extern void fs_check();
extern int fopen(char * name);
extern int kbread();
extern void printf(const char * fmt, ...);
extern int fread(void*,int,int,int);
extern int feof(int);