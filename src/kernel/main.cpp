#include <kernel/drivers/vga.h>

extern "C" void start() __attribute__((section(".text.kmain")));

extern "C" void start()
{
	vga::init();
    vga::write_line("[Kernel Entry]");

	__asm__ volatile("cli; hlt;");
}
