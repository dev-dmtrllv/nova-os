install_gdt:
	xor ax, ax									; SETUP GDT
	mov es, ax
	mov di, 0x800								; [es:di] -> [0x0:0x800]

	; null descriptor
	mov cx, 4
	rep stosw									; write 4 zero words at [es:di]

	; code descriptor
	mov [es:di], word 0xffff					; limit
	mov [es:di + 2], word 0x0					; base
	mov [es:di + 4], byte 0x0					; base 
	mov [es:di + 5], byte 0x9a					; access
	mov [es:di + 6], byte 0xcf					; flags and limit 
	mov [es:di + 7], byte 0x0					; base 
	add di, 8									; next descriptor

	; data descriptor
	mov [es:di], word 0xffff					; limit
	mov [es:di + 2], word 0x0					; base
	mov [es:di + 4], byte 0x0					; base 
	mov [es:di + 5], byte 0x92					; access
	mov [es:di + 6], byte 0xcf					; flags and limit 
	mov [es:di + 7], byte 0x0					; base

	lgdt [_gdt]

	ret



_gdt:
	dw 24					; size (24 bytes)
	dd 0x800				; entry address (0x0:0x800)
