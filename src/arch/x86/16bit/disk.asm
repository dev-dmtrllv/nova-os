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
