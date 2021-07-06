#include <kernel/drivers/vga.h>

#include <arch/x86/kinit/kmm.h>
#include <arch/x86/kinit/idt.h>
#include <arch/x86/kinit/isr.h>
#include <arch/x86/kinit/timer.h>
#include <arch/x86/kinit/keyboard.h>
#include <arch/x86/io.h>

#include <lib/string.h>

extern "C" void start() __attribute__((section(".text.kmain")));

void keydown(keyboard::scan_code code)
{
	char buf[2] = {keyboard::kc2c(code), '\0'};
	vga::write(buf);
}

extern "C" void start()
{
	vga::init();
	vga::clear();
	vga::write("[", vga::color::LIGHT_GREEN, vga::color::BLACK);
	vga::write("kinit entry");
	vga::write_line("]\n", vga::color::LIGHT_GREEN, vga::color::BLACK);
	kmm::init();

	vga::write("[", vga::color::LIGHT_GREEN, vga::color::BLACK);
	vga::write("memory manager initialized");
	vga::write_line("]\n", vga::color::LIGHT_GREEN, vga::color::BLACK);

	idt::init();
	vga::write_line("IDT initialized");

	timer::init(60);
	keyboard::init();

	keyboard::register_key_down(keydown);

	__asm__ volatile("sti");

	while (true)
	{
	}

	vga::write_line("stop");
	__asm__ volatile("cli; hlt;");
}