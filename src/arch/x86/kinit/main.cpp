#include <kernel/drivers/vga.h>
#include <arch/x86/kinit/kmm.h>
#include <arch/x86/kinit/idt.h>
#include <arch/x86/kinit/isr.h>
#include <arch/x86/io.h>
#include <lib/string.h>

extern "C" void start() __attribute__((section(".text.kmain")));

size_t i;

void timer_callback(isr::registers_t *regs)
{
	i++;
	char buf[16] = "";
	utoa(i, buf, 10);
	vga::write_at(buf, 80 - strlen(buf), 0);
	vga::write_at("tick ", (80 - strlen(buf)) - 6, 0);
}

void init_timer()
{
	i = 0;
	vga::write_line("init timer");
	isr::register_interrupt_handler(IRQ0, timer_callback);

	uint32_t divisor = 1193180 / 1;

	io::outb(0x43, 0x36);

	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	uint8_t l = (uint8_t)(divisor & 0xFF);
	uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

	// Send the frequency divisor.
	io::outb(0x40, l);
	io::outb(0x40, h);
}

void keyboard_callback(isr::registers_t* registers)
{
	unsigned char status = io::inb(0x64);
    unsigned char scancode = io::inb(0x60);
	char buf[16];
	utoa(scancode, buf, 10);
	vga::write_line("key interrupt ", buf);
}

void init_keyboard()
{
	isr::register_interrupt_handler(IRQ1, keyboard_callback);
}

extern "C" void start()
{
	vga::init();
	vga::clear();
	vga::write("[", vga::color::LIGHT_GREEN, vga::color::BLACK);
	vga::write("kinit entry");
	vga::write_line("]\n", vga::color::LIGHT_GREEN, vga::color::BLACK);
	kmm::init();

	vga::write("[", vga::color::LIGHT_GREEN, vga::color::BLACK);
	vga::write("memory manager initialized");
	vga::write_line("]\n", vga::color::LIGHT_GREEN, vga::color::BLACK);

	idt::init();
	vga::write_line("IDT initialized");

	__asm__ volatile("int $0x0");
	__asm__ volatile("int $0x1");
	__asm__ volatile("int $0x2");

	init_timer();
	init_keyboard();

	__asm__ volatile("sti");

	while (true) {}

	vga::write_line("stop");
	__asm__ volatile("cli; hlt;");
}