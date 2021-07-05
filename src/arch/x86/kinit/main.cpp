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

	__asm__ volatile("sti;");
	// __asm__ volatile("int $0x3");

	// __asm__ volatile("int $0x1");
	// __asm__ volatile("int $0x2");
	// __asm__ volatile("int $0x3");
	// __asm__ volatile("int $0x4");
	// __asm__ volatile("int $0x5");
	// __asm__ volatile("int $0x6");
	// __asm__ volatile("int $0x7");
	// __asm__ volatile("int $0x8");
	// __asm__ volatile("int $0x9");
	// __asm__ volatile("int $0xa");
	// __asm__ volatile("int $0xb");
	// __asm__ volatile("int $0xc");
	// __asm__ volatile("int $0x13");
	// __asm__ volatile("int $0x14");
	// __asm__ volatile("int $0x15");/

	vga::write_line("stop");
	__asm__ volatile("cli; hlt;");
}