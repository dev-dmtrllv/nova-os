[bits 16]
[org 0x1000]

%macro print_err_fc 2
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

	mov ax, 0x0002								; set video mode (clears the screen)
	int 0x10

	mov ax, 0x1003       						; turn blink off
    mov bl, 0
    int 0x10

	mov si, msg_boot_kernel
	call print_line

	mov word [file_name], kernel_ldr_img_name	; load kernel init image at 0x2000
	mov word [dap_buf_off], 0x2000
	mov word [dap_buf_seg], 0x0
	call read_file
	jc err_kernel_loader_missing

	mov word [file_name], kernel_img_name		; load kernel image at 0x3000
	mov word [dap_buf_off], 0x3000
	mov word [dap_buf_seg], 0x0
	call read_file
	jc err_kernel_missing

	pusha
	xor ax, ax
	mov es, ax
	mov di, 0x1500								; load the memory map before the kernel init
	call get_memory_map
	xor bp, bp
	mov [es:di + 2], word bp					; Write end of chain
	mov [es:di + 2], word bp
	mov [es:di + 2], word bp
	mov [es:di + 2], word bp
	mov [es:di + 2], word bp
	mov [es:di + 2], word bp
	popa

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

	lgdt [_gdt]
	lidt [_idt]


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
	mov esp, 0x1500  							; Set stack to grown downwards from 0x1500 (where the bios_mem_info list starts) 

	db 0x66
  	db 0xEA
  	dd 0x2000									; kernel offset
  	dw 0x8										; desciptor selector


; err reporing routines
print_err_fc 	err_kernel_missing, 		msg_kernel_not_found
print_err_fc 	err_a20, 					msg_a20_err
print_err_fc 	err_kernel_loader_missing,	msg_kinit_not_found

%include "16bit/common.asm"
%include "16bit/screen.asm"
%include "16bit/bpb.asm"
%include "16bit/disk.asm"
%include "16bit/a20.asm"
%include "16bit/memory.asm"

; data
msg_boot_kernel				db "Loading kernel init...", 0
msg_kernel_not_found		db "Kernel image not found!", 0
msg_kinit_not_found			db "kinit image not found!", 0
msg_a20_enabled				db "A20 enabled!", 0
msg_a20_err					db "A20 could not be enabled!", 0
msg_setup					db "Setting up the GDT & empty IDT...", 0

kernel_ldr_img_name			db "KINIT   IMG", 0
kernel_img_name				db "KERNEL  IMG", 0


_gdt:
	dw 24
	dd 0x800

_idt:
	dw 2048
	dd 0
