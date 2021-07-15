#include <arch/x86/isr.h>
#include <arch/x86/idt.h>
#include <arch/x86/io.h>
#include <kernel/drivers/vga.h>
#include <lib/stdint.h>
#include <lib/string.h>

#define PIC1 0x20 /* IO base address for master PIC */
#define PIC2 0xA0 /* IO base address for slave PIC */
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1 + 1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2 + 1)

extern "C" void isr0();	 // Division by zero exception
extern "C" void isr1();	 // Debug exception
extern "C" void isr2();	 // Non maskable interrupt
extern "C" void isr3();	 // Breakpoint exception
extern "C" void isr4();	 // 'Into detected overflow'
extern "C" void isr5();	 // Out of bounds exception
extern "C" void isr6();	 // Invalid opcode exception
extern "C" void isr7();	 // No coprocessor exception
extern "C" void isr8();	 // Double fault (pushes an error code)
extern "C" void isr9();	 // Coprocessor segment overrun
extern "C" void isr10(); // Bad TSS (pushes an error code)
extern "C" void isr11(); // Segment not present (pushes an error code)
extern "C" void isr12(); // Stack fault (pushes an error code)
extern "C" void isr13(); // General protection fault (pushes an error code)
extern "C" void isr14(); // Page fault (pushes an error code)
extern "C" void isr15(); // Unknown interrupt exception
extern "C" void isr16(); // Coprocessor fault
extern "C" void isr17(); // Alignment check exception
extern "C" void isr18(); // Machine check exception
extern "C" void isr19(); // reserved
extern "C" void isr20(); // reserved
extern "C" void isr21(); // reserved
extern "C" void isr22(); // reserved
extern "C" void isr23(); // reserved
extern "C" void isr24(); // reserved
extern "C" void isr25(); // reserved
extern "C" void isr26(); // reserved
extern "C" void isr27(); // reserved
extern "C" void isr28(); // reserved
extern "C" void isr29(); // reserved
extern "C" void isr30(); // reserved
extern "C" void isr31(); // reserved

extern "C" void irq0();	 // Division by zero exception
extern "C" void irq1();	 // Debug exception
extern "C" void irq2();	 // Non maskable interrupt
extern "C" void irq3();	 // Breakpoint exception
extern "C" void irq4();	 // 'Into detected overflow'
extern "C" void irq5();	 // Out of bounds exception
extern "C" void irq6();	 // Invalid opcode exception
extern "C" void irq7();	 // No coprocessor exception
extern "C" void irq8();	 // Double fault (pushes an error code)
extern "C" void irq9();	 // Coprocessor segment overrun
extern "C" void irq10(); // Bad TSS (pushes an error code)
extern "C" void irq11(); // Segment not present (pushes an error code)
extern "C" void irq12(); // Stack fault (pushes an error code)
extern "C" void irq13(); // General protection fault (pushes an error code)
extern "C" void irq14(); // Page fault (pushes an error code)
extern "C" void irq15(); // Unknown interrupt exception

namespace isr
{
	interrupt_handler_callback interrupt_handlers[265];
};

extern "C" void isr_handler(isr::registers_t *registers)
{
	if (isr::interrupt_handlers[registers->int_no] != 0)
	{
		isr::interrupt_handler_callback handler = isr::interrupt_handlers[registers->int_no];
		handler(registers);
	}
	else
	{
		char buf[16];
		utoa(registers->int_no, buf, 10);
		vga::write_line("isr ", buf, " recieved!");

		if (registers->int_no == 13)
		{
			utoa(registers->err_code, buf, 10);
			vga::write_line("general protection fault. err: ", buf);
			__asm__ volatile("hlt;");
		}
	}
}

extern "C" void irq_handler(isr::registers_t *registers)
{
	if (isr::interrupt_handlers[registers->int_no] != 0)
	{
		isr::interrupt_handler_callback handler = isr::interrupt_handlers[registers->int_no];
		handler(registers);
	}
	else
	{
		char buf[16];
		utoa(registers->int_no, buf, 10);
		vga::write_line("recieved unhandeld irq ", buf, "!");
	}

	// send End Of Interrupt (EOI) message
	if (registers->int_no >= 40)
	{
		// Send reset signal to slave.
		io::outb(0xA0, 0x20);
	}

	// Send reset signal to master. (As well as slave, if necessary).
	io::outb(0x20, 0x20);
}

void isr::init()
{
	memset(interrupt_handlers, 0, sizeof(interrupt_handler_callback) * 265);

	// remap the pit
	io::outb(0x20, 0x11);
	io::outb(0xA0, 0x11);
	io::outb(0x21, 0x20);
	io::outb(0xA1, 0x28);
	io::outb(0x21, 0x04);
	io::outb(0xA1, 0x02);
	io::outb(0x21, 0x01);
	io::outb(0xA1, 0x01);
	io::outb(0x21, 0x0);
	io::outb(0xA1, 0x0);

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

	idt::set_gate(32, (uint32_t)irq0, 0x08, 0x8E);
	idt::set_gate(33, (uint32_t)irq1, 0x08, 0x8E);
	idt::set_gate(34, (uint32_t)irq2, 0x08, 0x8E);
	idt::set_gate(35, (uint32_t)irq3, 0x08, 0x8E);
	idt::set_gate(36, (uint32_t)irq4, 0x08, 0x8E);
	idt::set_gate(37, (uint32_t)irq5, 0x08, 0x8E);
	idt::set_gate(38, (uint32_t)irq6, 0x08, 0x8E);
	idt::set_gate(39, (uint32_t)irq7, 0x08, 0x8E);
	idt::set_gate(40, (uint32_t)irq8, 0x08, 0x8E);
	idt::set_gate(41, (uint32_t)irq9, 0x08, 0x8E);
	idt::set_gate(42, (uint32_t)irq10, 0x08, 0x8E);
	idt::set_gate(43, (uint32_t)irq11, 0x08, 0x8E);
	idt::set_gate(44, (uint32_t)irq12, 0x08, 0x8E);
	idt::set_gate(45, (uint32_t)irq13, 0x08, 0x8E);
	idt::set_gate(46, (uint32_t)irq14, 0x08, 0x8E);
	idt::set_gate(47, (uint32_t)irq15, 0x08, 0x8E);

	for (size_t i = 0; i < 16; i++)
		set_irq_mask(i);
}

// register a callback to the
void isr::register_interrupt_handler(size_t index, isr::interrupt_handler_callback cb)
{
	interrupt_handlers[index] = cb;
	// we only need to clear the mask for all interrupts above the exceptions (exceptions are from 0 till 31)
	if (index >= 32)
		clear_irq_mask(index - 32);
}

// setting the mask will tell the pic to ignore this irq
void isr::set_irq_mask(unsigned char irq_line)
{
	uint16_t port;
	uint8_t value;

	if (irq_line < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq_line -= 8;
	}
	value = io::inb(port) | (1 << irq_line);
	io::outb(port, value);
}

// clearing the irq mask will tell the pic to stop ignoring this irq
void isr::clear_irq_mask(unsigned char irq_line)
{
	uint16_t port;
	uint8_t value;

	if (irq_line < 8)
	{
		port = PIC1_DATA;
	}
	else
	{
		port = PIC2_DATA;
		irq_line -= 8;
	}
	value = io::inb(port) & ~(1 << irq_line);
	io::outb(port, value);
}
