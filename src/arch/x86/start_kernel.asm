section .text

[bits 32]

global start_kernel32
global start_kernel64

start_kernel32:
	jmp 0x8:0x3000
    
start_kernel64:
	cli				; we are not set up to enter the 64bit world. 
	hlt				; if we remove this the computer will tripple fault and reset

    xor ax, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
	
	jmp 0x8:0x4000

	cli
    hlt
