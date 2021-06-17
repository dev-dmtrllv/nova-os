halt:
	cli
	hlt
	jmp $

; shutdown:
;     mov ax, 0x1000
;     mov ax, ss
;     mov sp, 0xf000
;     mov ax, 0x5307
;     mov bx, 0x0001
;     mov cx, 0x0003
;     int 0x15

; wait_for_enter:
;     mov ah, 0
;     int 0x16
;     cmp al, 0x0D
;     jne wait_for_enter
;     ret

err_exit:
	jmp halt
	; call wait_for_enter
	; call shutdown
