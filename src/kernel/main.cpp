#include <kernel/drivers/vga.h>

extern "C" void kmain()
{
	VGA::setColor(VGA::Color::GREEN);
	VGA::setBackground(VGA::Color::YELLOW);

	VGA::write("Hello, World!");

	__asm__("hlt");
}
