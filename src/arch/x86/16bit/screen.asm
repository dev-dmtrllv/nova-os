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
