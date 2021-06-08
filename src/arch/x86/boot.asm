[bits 16]
[org 0x7c00]

boot_start:
	xor ax, ax
	mov ds, ax
	mov es, ax
	mov bx, ax
	mov cx, ax
	mov dx, ax

	call clear_screen

	mov si, msg
	call print

	cli
	hlt

boot_loop:
	jmp boot_loop


clear_screen:
    pusha
  	mov ah, 0x00
  	mov al, 0x03  		; text mode 80x25 16 colours
  	int 0x10
  	popa
	ret


print:
    mov ah, 0x0e

.print_loop:
    lodsb
    cmp al, 0
    je .print_done
    int 0x10
    jmp .print_loop

.print_done:
    ret



msg:	db "loading first boot stage...", 0

times 510-($-$$) db 0
dw 0xAA55
