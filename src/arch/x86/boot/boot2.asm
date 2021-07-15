[bits 16]
[org 0x1000]

%define KINIT_SEG 0x1000
%define LIN_KINIT_ADDR 0x10000

%define KERNEL_SEG 0x3000
%define LIN_KERNEL_ADDR 0x30000

%define BIOS_KERNEL_INFO_AREA 0x3000

%define BOOT2 1

%macro print_err_fc 2
%1:
	mov si, %2
	call print_line
	jmp halt
%endmacro


start:
    cli
    mov	sp, stack_start
    cld
    sti
    jmp boot2
    nop

align 16
stack_end:
    times 1024 db 0
stack_start:

boot2:
    mov ax, 0x0002							; set video mode (clears the screen)
	int 0x10

	mov ax, 0x1003                          ; turn blink off
    mov bl, 0
    int 0x10

	mov si, msg_load_kernel
	call print_line

	pusha
	push ds
	push es
    mov word [file_name], kernel_file_name 	; load kernel
	mov word [dap_buf_off], 0x0000
	mov word [dap_buf_seg], KERNEL_SEG
	call read_file
	mov word [kernel_clusters], cx
	pop es
	pop ds
	popa
    jc err_kernel_missing

	mov si, msg_load_kinit
	call print_line

	pusha
	push ds
	push es
    mov word [file_name], kinit_file_name 	; load kinit
	mov word [dap_buf_off], 0x0000
	mov word [dap_buf_seg], KINIT_SEG
	call read_file
	pop es
	pop ds
	popa
    jc err_kinit_missing


	mov ax, 0xb101								; pci check
	int 0x1a
	cmp ah, 0
	je pci_available_check_1
	jmp set_boot_info							; else keep the pci support at 0

pci_available_check_1:
	xor ah, ah
	cmp edx, "PCI "
	je pci_available_check_2
	jmp set_boot_info
	
pci_available_check_2:
	mov byte [pci_support], al
	mov byte [pci_last_bus], cl

set_boot_info:
    pusha
	xor ax, ax
	mov es, ax
	mov di, memory_array						; load the memory map before the kernel init
	call get_memory_map
	mov di, BIOS_KERNEL_INFO_AREA				; store numbers of memory info list items
	mov [es:di], word bp
	mov [es:di + 2], word memory_array			; store pointer to the memory info list
	mov [es:di + 4], word kernel_size			; store pointer to the kernel size
	mov [es:di + 6], word pci_support			; pci support
	popa

    call check_a20								; check the a20 and enable
	cmp ax, 1
	je a20_enabled
	jmp err_a20

a20_enabled:
	mov si, msg_a20_enabled
	call print_line

	mov si, msg_setup_gdt
	call print_line

    cli
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

	xor ax, ax
	mov es, ax
	mov di, ax
	mov cx, 1024
	rep stosw

	lidt [_idt]
	
    mov	eax, cr0								; set cr0 PE-bit to enable 32bit
  	or eax, 1
  	mov cr0, eax

	jmp queue_cleared
    nop
    nop
    
print_err_fc	err_kinit_missing,			msg_kinit_not_found
print_err_fc 	err_kernel_missing, 		msg_kernel_not_found
print_err_fc 	err_a20, 					msg_a20_err

%include "16bit/common.asm"
%include "16bit/screen.asm"
%include "16bit/disk.asm"
%include "16bit/bpb.asm"
%include "16bit/memory.asm"
%include "16bit/a20.asm"


msg_load_kinit				db "Loading kinit image", 0
msg_load_kernel				db "Loading kernel image", 0
msg_a20_enabled				db "A20 enabled", 0
msg_kernel_not_found		db "Could not find kernel!", 0
msg_kinit_not_found			db "Could not find kinit!", 0
msg_a20_err					db "Could not enable A20 line!", 0
msg_setup_gdt				db "Installing GDT", 0
kinit_file_name				db "KINIT   BIN", 0
kernel_file_name            db "KERNEL  BIN", 0


_gdt:
	dw 24					; size (24 bytes)
	dd 0x800				; entry address (0x0:0x800)


_idt:
	dw 2048					; size (2048 bytes)
	dd 0					; entry adress (0x0:0x0)

queue_cleared:
	mov ax, 0x10								; set descriptor
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax
	mov esp, LIN_KINIT_ADDR - 8  			    ; Set stack to grown downwards from 0x10000 - 8 
	
	xor eax, eax								; now we can calculate the size of the kernel in bytesZ
	mov ecx, eax
	mov ax, word [bytesPerSector]
	mov cx, word [sectorsPerCluster]
	mul ecx
	xor ecx, ecx
	mov cx, word [kernel_clusters]
	mul ecx
	mov dword [kernel_size], eax

    jmp dword 0x8:LIN_KINIT_ADDR

memory_array: times 64 dd 0

kernel_size: dd 0

kernel_clusters: dd 0

pci_support: db 0
pci_last_bus: db 0
