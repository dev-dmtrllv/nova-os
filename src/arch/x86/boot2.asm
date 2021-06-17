[bits 16]
[org 0x8000]

boot_start_2:
	mov si, msg_hi
	call print
	call halt

%include "16bit/common.asm"
%include "16bit/screen.asm"

; data
msg_hi	db "Hello from 2nd boot stage!", 0
