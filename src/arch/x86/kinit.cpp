#include <kernel/drivers/vga.h>

extern "C" void kinit() __attribute__((section(".text.kinit")));

extern "C" void start_kernel32();
extern "C" void start_kernel64();

extern "C" void kinit()
{
	vga::set_background(vga::color::BLACK);
	vga::set_color(vga::color::LIGHT_GRAY);

	#ifdef KERNEL_BOOT_x86_64
	vga::write("Preparing for the 64bit kernel...\n");
	start_kernel64();
	#else
	start_kernel32();
	#endif
	
}
