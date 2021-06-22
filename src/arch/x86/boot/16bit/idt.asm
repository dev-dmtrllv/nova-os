install_idt:
	xor ax, ax
	mov es, ax
	mov di, ax
	mov cx, 1024
	rep stosw

	lidt [_idt]

	ret

_idt:
	dw 2048					; size (2048 bytes)
	dd 0					; entry adress (0x0:0x0)
