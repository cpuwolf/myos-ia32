.text
.globl _start
_start:
	pushl	%eax	#envir
	pushl	%ebx	#argv
	pushl	%ecx	#argc
	call	main
	addl	$12,%esp
	pushl	%eax
	call	_exit
	hlt
