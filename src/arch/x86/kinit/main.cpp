#include <kernel/drivers/vga.h>

#include <arch/x86/kinit/paging.h>

#include <arch/x86/acpi.h>
#include <arch/x86/pci.h>

#include <lib/string.h>

#include <arch/x86/boot/boot_info.h>

extern "C" void start() __attribute__((section(".text.kmain")));

extern "C" void start()
{
	vga::init();

	vga::write_line("Initializing paging");
	paging::init();

	vga::write_line("Initializing acpi");
	acpi::init();

	vga::write_line("Initializing pci");
	pci::init();

	vga::write_line("\n\nstop");
	__asm__ volatile("cli; hlt;");
}
