; check_drive_ext:
	; xor ax, ax
	; xor dx, dx
	; mov ah, 0x41
	; mov dl, byte [drive]
	; mov bx, 0x55aa
	; int 0x13
	; jc .drive_ext_err
	; cmp bx, 0xaa55
	; jnz .drive_ext_err
	; ret

; .drive_ext_err:
	; stc
	; ret





read_sectors:				; cx = number of sectors to load
	add cx, 1
.read_sectors_loop:
	mov ax, 0x4200
	xor dx, dx
	mov ds, dx
	mov dl, [drive]
	mov si, dap
	int 0x13
	jc .read_sectors_err
	dec cx
	cmp cx, 0
	jnz .read_next_sectors
	ret

.read_next_sectors:
	inc word [dap_lba]
	mov ax, word [dap_buf_off]
	add ax, word [bytesPerSector]
	mov word [dap_buf_off], ax
	jc .read_sectors_add_seg
	jmp .read_sectors_loop

.read_sectors_add_seg:
	add word [dap_buf_seg], 0x1000
	jmp .read_sectors_loop

.read_sectors_err:
	ret





find_file_start_cluster:					; set word [file_name] to point to file name; returns dx = cluster number
	mov ax, 0x0020                      	; 32 byte directory entry
	mul word [rootDirEntries]           	; total size of directory
	div word [bytesPerSector]        		; sectors used by directory
	xchg ax, cx								; set cx to root dir size (in sectors)
	; mov word [dap_sectors], cx

	mov al, byte [numberOfFats]				; calculate the directory table offset
	mul word [sectorsPerFat]				; fat tables * sectors per fat
	add ax, word [reservedSectors]			; add reserved sectors
	mov word [data_sector], ax				; set the data_sector pointer
	add word [data_sector], cx				; and add the root dir sectors to point to the first data sector
	mov word [dap_lba], ax					; prepare dap_lba to load the directory entries
	mov word [dap_buf_off], 0x8000
	mov word [dap_buf_seg], 0x0000

	call read_sectors
	
	mov cx, [rootDirEntries]
	mov di, 0x8000							; base pointer of directory entries

	xor ax, ax
	mov es, ax
	mov ds, ax

.find_file_loop:
	push cx
	mov cx, 11								; set cx to number of bytes to compare
	push di
	mov si, word [file_name]
	rep cmpsb								; compare [DS:SI], [ES:DI]
	pop di									; restore saved flags
	pop cx									
	je .file_found
	dec cx									; decrement the counter
	cmp cx, 0								; check if it was the last entry
	je .file_not_found						; if so error and exit
	add di, 0x20							; add 32 bytes to point to the next entry
	jmp .find_file_loop						; loop

.file_found:
	mov dx, [di + 0x001a]					; get byte 26 for the cluster number
	ret

.file_not_found:
	stc
	ret





load_cluster_chain:							; dx = start cluster; write all the cluster number from 0x7e00 up to 0x8000, returns cx = number of clusters
	push dx
 	mov cx, word [sectorsPerFat]			; load the fat table
 	mov ax, word [reservedSectors]			; offset is reserved sectors
 	mov word [dap_lba], ax					; point to the sector on disk
 	mov word [dap_buf_seg], 0x0				; set where to load in ram
	mov word [dap_buf_off], 0x8000			; set where to load in ram
 	call read_sectors
	pop dx
	
	mov bp, 0
.load_cluster_chain_loop:
	mov word [0x7e00 + bp], dx				; put the cluster number in the array
	mov ax, dx
	shl dx, 1								; multiply by 2 cus its FAT 16 <- 2 * 8
	mov di, 0x8000
	add di, dx								; point to the next cluster number
	mov dx, word [di]						; dx is not the next cluster number
	mov bx, 0xfff8							; check for last cluster
	add bp, 2
	cmp bx, dx
	jbe .load_cluster_chain_done
	jmp .load_cluster_chain_loop

.load_cluster_chain_done:
	mov cx, bp
	shr cx, 1
	ret





cluster_to_lba: ; ax = cluster number, returns ax = lba 
	push cx
	sub ax, 0x0002
	xor cx, cx
	mov cl, byte [sectorsPerCluster]
	mul cx
	add ax, WORD [data_sector]
	pop cx
	ret




read_file: 							; set [file_name] to point to the filename string
	push word [dap_buf_seg]
	push word [dap_buf_off]
	call find_file_start_cluster	; dx now contains the first cluster number
	jc .read_file_not_found
	call load_cluster_chain			; load all the cluster numbers at 0x7e00, cx contains the number of clusters

	mov bp, 0 
	pop word [dap_buf_off]
	pop word [dap_buf_seg]

.read_file_cluster:
	mov ax, [0x7e00 + bp]			; point to the cluster number
	call cluster_to_lba
	mov word [dap_lba], ax 
	push cx
	xor cx, cx
	mov cl, byte [sectorsPerCluster]
	call read_sectors
	pop cx
	add bp, 2
	loop .read_file_cluster
	ret
	
.read_file_not_found:
	pop ax
	pop ax
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

file_name:					dw 0x0