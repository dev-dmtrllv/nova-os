#include <kernel/drivers/vga.h>

// set this method at the beginning of the img
extern "C" void kmain() __attribute__((section(".text.kmain")));

extern "C" void kmain()
{
	volatile char* ptr = (volatile char*)0xb8000;
	*ptr = 'X';

	VGA::setColor(VGA::Color::GREEN);
	VGA::setBackground(VGA::Color::YELLOW);

	VGA::write("Hello, World!");

	__asm__("hlt");
}
