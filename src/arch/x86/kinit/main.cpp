#include <kernel/drivers/vga.h>
#include <arch/x86/kinit/kmm.h>
#include <arch/x86/kinit/idt.h>
#include <lib/string.h>

extern "C" void start() __attribute__((section(".text.kmain")));

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

	vga::write_line("stop");
	__asm__ volatile("cli; hlt;");
}