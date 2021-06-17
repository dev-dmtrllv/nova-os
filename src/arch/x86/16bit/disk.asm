check_drive_ext:
	xor ax, ax
	xor dx, dx
	mov ah, 0x41
	mov dl, [drive]
	mov bx, 0x55AA
	int 0x13
	ret

cluster_to_lba: ; (ax) lba = (cluster_num - 2) * num_of_sect_in_cluster
	push cx
	sub ax, 0x0002
	xor cx, cx
	mov cl, byte [sectorsPerCluster]
	mul cx
	add ax, WORD [data_sector]
	pop cx
	ret

read_sectors: ; cx = number of sectors, dx = pointer to buffer
	pusha
	mov word [dap_buf_off], dx
	xor ax, ax
	xor dx, dx
	mov ds, dx
	mov ah, 0x42
	mov dl, byte [drive]
	mov si, dap
	int 0x13
	jc .read_sectors_failed
	popa
	add dx, 0x200	; add 512 bytes
	jc .read_sectors_add_seg
	loop read_sectors
	ret
.read_sectors_add_seg:
	mov ax, word [dap_buf_seg]
	add ax, 0x1000
	mov word [dap_buf_seg], ax
	loop read_sectors
	ret

.read_sectors_failed:
	; popa
	; xor ax, ax
	; mov es, ax
	; mov si, msg_read_sector_failed
	; call print_line
	; stc
	ret




read_file:
	mov word [file_name], ax
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

find_file_loop:
	push cx
	mov cx, 11								; set cx to number of bytes to compare
	mov si, word [file_name]				; compare with the image name
	push di
	rep cmpsb								; compare [DS:SI], [ES:DI]
	pop di									; restore saved flags
	je found_file
	pop cx									
	cmp cx, 0								; check if it was the last entry
	je file_not_found						; if so error and exit
	dec cx									; decrement the counter
	add di, 0x20							; add 32 bytes to point to the next entry
	jmp find_file_loop						; loop

found_file:
	mov dx, [di + 0x001a]					; get byte 26 for the cluster number
	mov ax, 0x7c00
	mov sp, ax
	push dx
	mov bp, sp
	mov word [file_cluster_start], bp
	
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
	mov bp, word [file_cluster_start]		; point to the first cluster adress
	mov dx, word [file_buff_seg]
	mov word [dap_buf_seg], dx
	mov dx, word [file_buff_off]			; load file from 0x0000:0x8000

load_file_cluster:
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
	jbe load_file_cluster_done
	sub bp, 2
	jmp load_file_cluster

load_file_cluster_done:
	clc
	ret

file_not_found:
	stc
	ret



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
data_sector:				dw 0x0
; msg_read_sector_failed:		db "Reading sector failed!", 0
file_name:					dw 0x0
file_cluster_start:			dw 0x0
file_buff_off:				dw 0x0
file_buff_seg:				dw 0x0
