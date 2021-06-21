#include <kernel/drivers/vga.h>
#include <kernel/io.h>

// set this method at the beginning of the img
extern "C" void kmain() __attribute__((section(".text.kmain")));

extern "C" void kmain()
{
	vga::set_background(vga::color::BLACK);
	vga::set_background(vga::color::BLACK);
	vga::set_background(vga::color::BLACK);
	
	vga::set_color(vga::color::LIGHT_GREEN);
	vga::write("[");
	
	vga::set_color(vga::color::LIGHT_GRAY);
	vga::write("Kernel Entry");

	vga::set_color(vga::color::LIGHT_GREEN);
	vga::write("]\n");

	__asm__ volatile("cli; hlt;");
}
