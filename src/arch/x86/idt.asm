[bits 32]

global idt_flush

idt_flush: ; load the idt pointer
    mov eax, [esp + 4]
    lidt [eax]
    ret