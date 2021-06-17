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
	
											; load the directory entries
    mov ax, 0x0020                      	; 32 byte directory entry
    mul word [rootDirEntries]           	; total size of directory
    div word [bytesPerSector]        		; sectors used by directory
	xchg ax, cx								; set cx to root dir size (in sectors)
	mov word [dap_sectors], cx

	mov al, byte [numberOfFats]				; calculate the directory table offset
	mul word [sectorsPerFat]				; fat tables * sectors per fat
	add ax, word [reservedSectors]			; add reserved sectors
	mov word [data_sector], ax				; set the data_sector pointer
	add word [data_sector], cx				; and add the root dir sectors to point to the first data sector
	mov word [dap_lba], ax					; prepare dap_lba to load the directory entries
	mov dx, 0x7e00							; set where to load in ram
	call read_sectors

	mov cx, [rootDirEntries]				; number of tries to find 2nd boot loader image (the kernel loader)
	mov di, 0x7e00							; base pointer of directory entries

find_img_loop:
	push cx
	mov cx, 11								; set cx to number of bytes to compare
	mov si, img_name						; compare with the image name
	push di
	rep cmpsb								; compare [DS:SI], [ES:DI]
	pop di									; restore saved flags
	je found_img
	pop cx									
	cmp cx, 0								; check if it was the last entry
	je img_not_found						; if so error and exit
	dec cx									; decrement the counter
	add di, 0x20							; add 32 bytes to point to the next entry
	jmp find_img_loop						; loop

found_img:
	mov dx, [di + 0x001a]					; get byte 26 for the cluster number
	mov ax, 0x7c00
	mov sp, ax
	push dx
	mov bp, sp
	mov word [img_cluster_start], bp
	
	mov cx, word [sectorsPerFat]			; load the fat table
	mov ax, word [reservedSectors]			; offset is reserved sectors 
	mov word [dap_lba], ax					; point to the sector on disk
	mov dx, 0x7e00							; set where to load in ram
	call read_sectors

load_cluster_chain:
	mov bp, sp								; point to the last cluster on the stack
	mov dx, word [bp]						; load the last cluster number
	shl dx, 0x1								; multiply by two because FAT16 uses 2 bytes
	mov di, 0x7e00							; restore di to point to the fat table
	add di, dx								; add the last cluster number to the fat table address 
	mov dx, word [di]						; get the new cluster number
	push dx									; and push it on the stack
	mov cx, 0xfff8							; check for last cluster
	cmp cx, dx
	jbe load_cluster_chain_done
	jmp load_cluster_chain

load_cluster_chain_done:
	mov bp, word [img_cluster_start]		; point to the first cluster adress
	mov word [dap_buf_seg], 0x0				; reset dap segment
	mov dx, 0x8000							; load file from 0x0000:0x8000

load_img_cluster:
	mov ax, word [bp]						; get the first cluster number
	push dx
	call cluster_to_lba
	pop dx
	mov word [dap_lba], ax
	xor cx, cx
	mov cl, byte [sectorsPerCluster]
	call read_sectors
	mov ax, word [bp]
	mov bx, 0xfff8
	cmp bx, ax
	jbe load_img_cluster_done
	sub bp, 2
	jmp load_img_cluster



load_img_cluster_done:
	xor ax, ax
	mov es, ax
	mov ds, ax
	jmp 0x0000:0x8000
	jmp halt


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
img_cluster_start:			dw 0x0

times 510-($-$$) db 0
dw 0xAA55
