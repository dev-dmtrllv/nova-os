[bits 16]
[org 0x1000]

%macro print_fc 2
%1:
	mov si, %2
	call print_line
	jmp halt
%endmacro

boot_start_2:
	cli											; setup the new stack
	mov ax, 0x1000
	mov sp, ax
	mov bp, ax
	sti

	MOV ax, 0x1003       						; turn blink off
    MOV bl, 0
    INT 0x10

	mov si, msg_boot_kernel
	call print_line

	mov word [file_name], kernel_img_name		; load kernel image at 0x10000
	mov word [dap_buf_off], 0x0
	mov word [dap_buf_seg], 0x1000
	
	call read_file
	jc err_kernel_missing


	call check_a20								; check the a20 and enable
	cmp ax, 1
	je a20_enabled
	jmp err_a20



a20_enabled:
	mov si, msg_a20_enabled
	call print_line

	mov si, msg_setup
	call print_line

	cli											; setup GDT and IDT

	xor ax, ax
	mov es, ax
	mov di, 0x800

null_desc:
	mov cx, 4
	rep stosw									; write 4 zero words

code_desc:
	mov [es:di], word 0xffff					; limit
	mov [es:di + 2], word 0x0					; base
	mov [es:di + 4], byte 0x0					; base 
	mov [es:di + 5], byte 0x9a					; access
	mov [es:di + 6], byte 0xcf					; flags and limit 
	mov [es:di + 7], byte 0x0					; base 
	add di, 8									; next descriptor

	mov [es:di], word 0xffff					; limit
	mov [es:di + 2], word 0x0					; base
	mov [es:di + 4], byte 0x0					; base 
	mov [es:di + 5], byte 0x92					; access
	mov [es:di + 6], byte 0xcf					; flags and limit 
	mov [es:di + 7], byte 0x0					; base

	lgdt [gdt]
	lidt [idt]


	mov	eax, cr0								; set cr0 PE-bit to enable 32bit
  	or eax, 1
  	mov cr0, eax

	jmp queue_cleared
    nop
    nop

queue_cleared:
	mov ax, 0x10								; set descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, 0xffff  							; Set stack to grown downwards from 0x10000

	db 0x66
  	db 0xEA
  	dd 0x10000            						; kernel offset
  	dw 0x0008             						; desciptor selector

	jmp halt


; err reporing routines
print_fc 	err_kernel_missing, 	msg_kernel_not_found
print_fc 	err_a20, 				msg_a20_err

%include "16bit/common.asm"
%include "16bit/screen.asm"
%include "16bit/bpb.asm"
%include "16bit/disk.asm"
%include "16bit/a20.asm"

; data
msg_boot_kernel			db "Loading kernel...", 0
msg_kernel_not_found	db "Kernel image not found!", 0
msg_a20_enabled			db "A20 enabled!", 0
msg_a20_err				db "A20 could not be enabled!", 0
msg_setup				db "Setting up the GDT & IDT...", 0

kernel_img_name			db "KERNEL  IMG", 0


gdt:
	dw 24
	dd 0x800

idt:
	dw 2048
	dd 0
