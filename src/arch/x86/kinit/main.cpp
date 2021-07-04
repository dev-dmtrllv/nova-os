#include <kernel/drivers/vga.h>
#include <arch/x86/kinit/kmm.h>
#include <lib/string.h>

extern "C" void start() __attribute__((section(".text.kmain")));

extern "C" void start()
{
	vga::init();
	vga::write("[", vga::color::LIGHT_GREEN, vga::color::BLACK);
	vga::write("kinit entry");
	vga::write_line("]", vga::color::LIGHT_GREEN, vga::color::BLACK);
	kmm::init();

	__asm__ volatile("cli; hlt;");
}