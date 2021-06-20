#include <kernel/drivers/vga.h>
#include <kernel/io.h>

// set this method at the beginning of the img
extern "C" void kmain() __attribute__((section(".text.kmain")));

extern "C" void kmain()
{
	vga::clear(vga::color::BLACK);
	vga::set_color(vga::color::LIGHT_GREEN);
	vga::set_background(vga::color::BLACK);

	vga::write("Kernel booted in 32bit. Checking for 64bit support...\n");
}
