#include <kernel/drivers/vga.h>

// set this method at the beginning of the img
extern "C" void kmain() __attribute__((section(".text.kmain")));

extern "C" void kmain()
{
	volatile char* ptr = (volatile char*)0xb8000;
	ptr += 160;
	*ptr = 'X';

	vga::set_color(vga::color::GREEN);
	vga::set_background(vga::color::YELLOW);

	vga::write("Hello, World!");

	__asm__("hlt");
}
