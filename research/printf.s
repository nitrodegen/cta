global _start

section .text

_start:
	mov rax,1; output
	mov rdi,1; open for writing to stdout
	mov rsi,name ; moving data to buffer
name:
	db "hello"


	mov rdx,5 ; length of data
	syscall ; syscall int 0x80
	mov rax,60 ; term sig
	mov rdi,0 ; ret 0 
	syscall ; int 0x80
	ret; return 


