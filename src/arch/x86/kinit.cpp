#include <kernel/drivers/vga.h>
#include <lib/string.h>

extern "C" void kinit() __attribute__((section(".text.kinit")));

extern "C" void start_kernel32();
extern "C" void start_kernel64();

extern "C" void kinit()
{
	vga::set_background(vga::color::BLACK);
	vga::set_color(vga::color::LIGHT_GRAY);

	char buf[12];

	for (size_t j = 0; j < 100; j++)
		for (size_t i = 0; i < 80; i++)
			vga::write(itoa(i*j, buf, 10));
	
	vga::write("\n");

#ifdef KERNEL_BOOT_x86_64
	vga::write("Preparing for the 64bit kernel...\n");
	start_kernel64();
#else
	start_kernel32();
#endif

}
