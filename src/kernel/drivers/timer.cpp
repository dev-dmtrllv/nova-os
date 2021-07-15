#include <kernel/drivers/timer.h>

#include <arch/x86/isr.h>
#include <arch/x86/io.h>

#include <kernel/drivers/vga.h>

#include <lib/string.h>

namespace timer
{
	uint64_t tick_count;

	void timer_callback(isr::registers_t *registers)
	{
		tick_count++;
		char buf[16] = "";
		ltoa(tick_count, buf, 10);
		vga::write_at(buf, 80 - strlen(buf), 0);
		vga::write_at("tick ", (80 - strlen(buf)) - 6, 0);
	}
};

void timer::init(size_t frequency)
{
	vga::write_line("initializing timer");

	tick_count = 0;
	isr::register_interrupt_handler(IRQ0, timer_callback);

	uint32_t divisor = 1193180 / frequency;

	io::outb(0x43, 0x36);

	// Divisor has to be sent byte-wise, so split here into upper/lower bytes.
	uint8_t l = (uint8_t)(divisor & 0xFF);
	uint8_t h = (uint8_t)((divisor >> 8) & 0xFF);

	// Send the frequency divisor.
	io::outb(0x40, l);
	io::outb(0x40, h);

	vga::write_line("timer initialized");
}
