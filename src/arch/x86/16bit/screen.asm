print:
	lodsb
	or al, al
	jz .print_done
	mov ah, 0x0e
	int 0x10
	jmp print
.print_done:
	ret

print_line:
	call print
	mov si, msg_EOL
	call print
	ret

; data
msg_EOL 	db 13, 10, 0
