[bits 16]
[org 0x7c00]

boot_start:
	jmp short boot_resume
	nop

;-----------------------------------;
;	Bios Parameter Block (FAT16)	;
;-----------------------------------; 
OEMLabel				db "NOVABOOT"		; Disk label
bytesPerSector			dw 512				; bytes per sector
sectorsPerCluster		db 1				; Sectors per cluster
reservedSectors			dw 1				; Reserved sectors for boot record (not included in the root directory / the file system)
numberOfFats			db 2				; 2 FAT tables to prevent data loss
rootDirEntries			dw 512				; Number of entries in root dir
smallNumberOfSectors	dw 0				; Number of small sectors
mediaDescriptor			db 0xf8				; Medium descriptor byte (0xf8 = fixed disk)
sectorsPerFat			dw 25				; Sectors per FAT
sectorsPerTrack			dw 63				; Sectors per track (36/cylinder)
headsPerCylinder		dw 255				; Number of sides/heads
hiddenSectors			dd 0				; Number of hidden sectors
largeSectors			dd 1000				; Number of large sectors
drive					dw 0x80				; Drive Number
signature				db 0x29				; Drive signature
volumeID				dd 0x1234abcd		; Volume ID
volumeLabel				db "NOVAOS     "	; Volume Label (11 chars)
fileSystem				db "FAT16   "		; File system type

boot_resume:
	cli
	xor ax, ax								; zero out all registers
	mov ds, ax
	mov es, ax
	xor dh, dh
	mov ss, ax								; setup the stack
	
	mov [drive], dl							; store the drive number
	
	mov ax, 0x7c00
   	mov sp, ax	 							; stack pointer (grows downwards from 0x7c00 -> 0x0000)
	cld										; clear direction flag
	sti										; start interrupts


	mov word [dap_buf_seg], 0x0
	mov word [dap_buf_off], 0x1000
	mov cx, word [reservedSectors]
	mov word [dap_sectors], cx
	mov word [dap_lba], 0x1					; from the 2nd sector
	mov ax, 0x4200							; read sectors
	mov dl, [drive]
	mov si, dap
	int 0x13
	jmp 0x1000

%include "16bit/screen.asm"
%include "16bit/common.asm"

;-----------------------------------;
;				DATA				;
;-----------------------------------; 
dap:
dap_size: 					db 0x10
dap_reserved:				db 0x0
dap_sectors:				dw 0x1
dap_buf_off:				dw 0x7e00
dap_buf_seg:				dw 0x0
dap_lba:					dd 0x0
							dd 0x0

times 510-($-$$) db 0
dw 0xAA55
