enable_32bit:
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
	mov esp, 0x1000 - 4  						; Set stack to grown downwards from 0x1000
	ret
