#include <kernel/drivers/vga.h>
#include <arch/x86/kinit/kmm.h>

extern "C" void start() __attribute__((section(".text.kmain")));

extern "C" void start()
{
	vga::init();

	kmm::init();

	__asm__ volatile("cli; hlt;");
}