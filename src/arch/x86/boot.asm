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
	mov fs, ax
	mov bx, ax
	mov cx, ax
	xor dh, dh
	mov [drive], dl							; store the drive number
	mov dx, ax
	
	mov ss, ax								; setup the stack
	mov ax, 0x7c00
   	mov sp, ax	 							; stack pointer (grows downwards from 0x7c00 -> 0x0000)
	cld										; clear direction flag
	sti										; start interrupts

	call check_drive_ext					; check if we can use the bios drive extension to read from the drive
	; jc disk_ext_err						; if the extension is not supported the carry flag is set
	jc err_exit
	cmp bx, 0xAA55							; bx should stay 0xAA55
	; jne disk_ext_err						; else show the error message and stop
	jc err_exit


	mov ax, img_name
	mov word [file_buff_off], 0x8000
	mov word [file_buff_seg], 0x0 
	call read_file
	jc img_not_found

	hlt



disk_ext_err:
	mov si, msg_drive_ext_fail
	call print
	jmp err_exit

img_not_found:
	mov si, msg_img_not_found
	call print
	jmp err_exit

%include "16bit/screen.asm"
%include "16bit/disk.asm"
%include "16bit/common.asm"

;-----------------------------------;
;				DATA				;
;-----------------------------------; 
msg_drive_ext_fail:			db "NO EXT!", 0
msg_img_not_found:			db "NO IMG!", 0
img_name:					db "NOVALDR BIN", 0

times 510-($-$$) db 0
dw 0xAA55
