#include <arch/x86/kinit/isr.h>
#include <arch/x86/kinit/idt.h>
#include <kernel/drivers/vga.h>
#include <lib/stdint.h>
#include <lib/string.h>

namespace isr
{
    /*
    exceptions:
        0 - Division by zero exception
        1 - Debug exception
        2 - Non maskable interrupt
        3 - Breakpoint exception
        4 - 'Into detected overflow'
        5 - Out of bounds exception
        6 - Invalid opcode exception
        7 - No coprocessor exception
        8 - Double fault (pushes an error code)
        9 - Coprocessor segment overrun
        10 - Bad TSS (pushes an error code)
        11 - Segment not present (pushes an error code)
        12 - Stack fault (pushes an error code)
        13 - General protection fault (pushes an error code)
        14 - Page fault (pushes an error code)
        15 - Unknown interrupt exception
        16 - Coprocessor fault
        17 - Alignment check exception
        18 - Machine check exception
        19 - 31 reserved
    */

    struct registers_t
    {
        uint32_t ds;                                     // Data segment selector
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
        uint32_t int_no, err_code;                       // Interrupt number and error code (if applicable)
        uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically.
    };
};

extern "C" void isr0();
extern "C" void isr1();
extern "C" void isr2();
extern "C" void isr3();
extern "C" void isr4();
extern "C" void isr5();
extern "C" void isr6();
extern "C" void isr7();
extern "C" void isr8();
extern "C" void isr9();
extern "C" void isr10();
extern "C" void isr11();
extern "C" void isr12();
extern "C" void isr13();
extern "C" void isr14();
extern "C" void isr15();
extern "C" void isr16();
extern "C" void isr17();
extern "C" void isr18();
extern "C" void isr19();
extern "C" void isr20();
extern "C" void isr21();
extern "C" void isr22();
extern "C" void isr23();
extern "C" void isr24();
extern "C" void isr25();
extern "C" void isr26();
extern "C" void isr27();
extern "C" void isr28();
extern "C" void isr29();
extern "C" void isr30();
extern "C" void isr31();


extern "C" void isr_handler(isr::registers_t registers)
{
    char buf[16];
    utoa(registers.int_no, buf, 10);
    vga::write_line("exception recieved! ", buf);

    if(registers.int_no == 13)
    {
        utoa(registers.err_code, buf, 10);
        vga::write_line("general protection fault. err: ", buf);
        __asm__ volatile("hlt;");
    }
}

void isr::init()
{
    idt::set_gate(0, (uint32_t)isr0, 0x08, 0x8E);
    idt::set_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    idt::set_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    idt::set_gate(3, (uint32_t)isr3, 0x08, 0x8E);
    idt::set_gate(4, (uint32_t)isr4, 0x08, 0x8E);
    idt::set_gate(5, (uint32_t)isr5, 0x08, 0x8E);
    idt::set_gate(6, (uint32_t)isr6, 0x08, 0x8E);
    idt::set_gate(7, (uint32_t)isr7, 0x08, 0x8E);
    idt::set_gate(8, (uint32_t)isr8, 0x08, 0x8E);
    idt::set_gate(9, (uint32_t)isr9, 0x08, 0x8E);
    idt::set_gate(10, (uint32_t)isr10, 0x08, 0x8E);
    idt::set_gate(11, (uint32_t)isr11, 0x08, 0x8E);
    idt::set_gate(12, (uint32_t)isr12, 0x08, 0x8E);
    idt::set_gate(13, (uint32_t)isr13, 0x08, 0x8E);
    idt::set_gate(14, (uint32_t)isr14, 0x08, 0x8E);
    idt::set_gate(15, (uint32_t)isr15, 0x08, 0x8E);
    idt::set_gate(16, (uint32_t)isr16, 0x08, 0x8E);
    idt::set_gate(17, (uint32_t)isr17, 0x08, 0x8E);
    idt::set_gate(18, (uint32_t)isr18, 0x08, 0x8E);
    idt::set_gate(19, (uint32_t)isr19, 0x08, 0x8E);
    idt::set_gate(20, (uint32_t)isr20, 0x08, 0x8E);
    idt::set_gate(21, (uint32_t)isr21, 0x08, 0x8E);
    idt::set_gate(22, (uint32_t)isr22, 0x08, 0x8E);
    idt::set_gate(23, (uint32_t)isr23, 0x08, 0x8E);
    idt::set_gate(24, (uint32_t)isr24, 0x08, 0x8E);
    idt::set_gate(25, (uint32_t)isr25, 0x08, 0x8E);
    idt::set_gate(26, (uint32_t)isr26, 0x08, 0x8E);
    idt::set_gate(27, (uint32_t)isr27, 0x08, 0x8E);
    idt::set_gate(28, (uint32_t)isr28, 0x08, 0x8E);
    idt::set_gate(29, (uint32_t)isr29, 0x08, 0x8E);
    idt::set_gate(30, (uint32_t)isr30, 0x08, 0x8E);
    idt::set_gate(31, (uint32_t)isr31, 0x08, 0x8E);
}