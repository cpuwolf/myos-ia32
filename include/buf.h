/*
*	written by wei shuai
*	2004/9/27
*
*/
#ifndef _OS_BUF_H
#define _OS_BUF_H

#define BLOCK_SIZE 1024

struct buf
{
	unsigned char data[BLOCK_SIZE];
};

#endif