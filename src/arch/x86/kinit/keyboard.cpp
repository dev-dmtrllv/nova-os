#include <arch/x86/kinit/keyboard.h>
#include <arch/x86/io.h>
#include <arch/x86/kinit/isr.h>

#include <kernel/drivers/vga.h>

#include <lib/string.h>

#define SCAN_CODE_SIZE 85

namespace keyboard
{
	bool key_down_info[SCAN_CODE_SIZE];

	char scan_codes_map[SCAN_CODE_SIZE - 1];

	keyboard_event_callback key_up_listeners[32];
	keyboard_event_callback key_down_listeners[32];

	void keyboard_callback(isr::registers_t *registers)
	{
		unsigned char code = io::inb(0x60);

		unsigned char released = static_cast<unsigned char>(keyboard::scan_code::RELEASED);

		//char buf[6];
		//utoa(code, buf, 16);

		//vga::write_line("\n",buf);

		if (code >= released)
		{
			key_down_info[code - released] = false;

			for (size_t i = 0; i < 32; i++)
				if (key_up_listeners[i] != 0)
					key_up_listeners[i](static_cast<keyboard::scan_code>(code));
		}
		else
		{
			key_down_info[code] = true;

			for (size_t i = 0; i < 32; i++)
				if (key_down_listeners[i] != 0)
					key_down_listeners[i](static_cast<keyboard::scan_code>(code));
		}
	}
};

void keyboard::init()
{
	memset(key_down_info, 0, sizeof(bool) * SCAN_CODE_SIZE);
	memset(scan_codes_map, 0, sizeof(char) * (SCAN_CODE_SIZE - 1));

	memset(key_up_listeners, 0, sizeof(keyboard_event_callback) * 32);
	memset(key_down_listeners, 0, sizeof(keyboard_event_callback) * 32);
	
	scan_codes_map[0x0] = '\0';
	scan_codes_map[0x1] = '1';
	scan_codes_map[0x2] = '2';
	scan_codes_map[0x3] = '3';
	scan_codes_map[0x4] = '4';
	scan_codes_map[0x5] = '5';
	scan_codes_map[0x6] = '6';
	scan_codes_map[0x7] = '7';
	scan_codes_map[0x8] = '8';
	scan_codes_map[0x9] = '9';
	scan_codes_map[0xa] = '0';
	scan_codes_map[0xb] = '-';
	scan_codes_map[0x0C] = '=';
	scan_codes_map[0x0D] = '\b';
	scan_codes_map[0x0E] = '\0';
	scan_codes_map[0x0F] = 'q';
	scan_codes_map[0x10] = 'w';
	scan_codes_map[0x11] = 'e';
	scan_codes_map[0x12] = 'r';
	scan_codes_map[0x13] = 't';
	scan_codes_map[0x14] = 'y';
	scan_codes_map[0x15] = 'u';
	scan_codes_map[0x16] = 'i';
	scan_codes_map[0x17] = 'o';
	scan_codes_map[0x18] = 'p';
	scan_codes_map[0x19] = ']';
	scan_codes_map[0x1A] = '[';
	scan_codes_map[0x1B] = '\0';
	scan_codes_map[0x1C] = '\0';
	scan_codes_map[0x1D] = 'a';
	scan_codes_map[0x1E] = 's';
	scan_codes_map[0x1F] = 'd';
	scan_codes_map[0x20] = 'f';
	scan_codes_map[0x21] = 'g';
	scan_codes_map[0x22] = 'h';
	scan_codes_map[0x23] = 'j';
	scan_codes_map[0x24] = 'k';
	scan_codes_map[0x25] = 'l';
	scan_codes_map[0x26] = ';';
	scan_codes_map[0x27] = '\'';
	scan_codes_map[0x28] = '`';
	scan_codes_map[0x29] = '\0';
	scan_codes_map[0x2A] = '\0';
	scan_codes_map[0x2B] = 'z';
	scan_codes_map[0x2C] = 'x';
	scan_codes_map[0x2D] = 'c';
	scan_codes_map[0x2E] = 'v';
	scan_codes_map[0x2F] = 'b';
	scan_codes_map[0x30] = 'n';
	scan_codes_map[0x31] = 'm';
	scan_codes_map[0x32] = ',';
	scan_codes_map[0x33] = '.';
	scan_codes_map[0x34] = '/';
	scan_codes_map[0x35] = '\0';
	scan_codes_map[0x36] = '\0';
	scan_codes_map[0x37] = '\0';
	scan_codes_map[0x38] = ' ';
	scan_codes_map[0x39] = '\0';
	scan_codes_map[0x3A] = '\0';
	scan_codes_map[0x3B] = '\0';
	scan_codes_map[0x3C] = '\0';
	scan_codes_map[0x3D] = '\0';
	scan_codes_map[0x3E] = '\0';
	scan_codes_map[0x3F] = '\0';
	scan_codes_map[0x40] = '\0';
	scan_codes_map[0x41] = '\0';
	scan_codes_map[0x42] = '\0';
	scan_codes_map[0x43] = '\0';
	scan_codes_map[0x44] = '\0';
	scan_codes_map[0x45] = '\0';
	scan_codes_map[0x46] = '7';
	scan_codes_map[0x47] = '8';
	scan_codes_map[0x48] = '9';
	scan_codes_map[0x49] = '-';
	scan_codes_map[0x4A] = '4';
	scan_codes_map[0x4B] = '5';
	scan_codes_map[0x4C] = '6';
	scan_codes_map[0x4D] = '+';
	scan_codes_map[0x4E] = '1';
	scan_codes_map[0x4F] = '2';
	scan_codes_map[0x50] = '3';
	scan_codes_map[0x51] = '0';
	scan_codes_map[0x52] = '.';
	scan_codes_map[0x53] = '\0';
	scan_codes_map[0x57] = '\0';
	scan_codes_map[0x58] = '\0';

	isr::register_interrupt_handler(IRQ1, keyboard::keyboard_callback);
}

char keyboard::kc2c(keyboard::scan_code code)
{
	unsigned char c = static_cast<unsigned char>(code);
	unsigned char released = static_cast<unsigned char>(keyboard::scan_code::RELEASED);

	if (c >= released)
		return scan_codes_map[c - released - 1];
	return scan_codes_map[c - 1];
}

void keyboard::register_key_down(keyboard::keyboard_event_callback callback)
{
	for (size_t i = 0; i < 32; i++)
		if (key_down_listeners[i] == 0)
		{
			key_down_listeners[i] = callback;
			return;
		}
}

void keyboard::register_key_up(keyboard::keyboard_event_callback callback)
{
	for (size_t i = 0; i < 32; i++)
		if (key_up_listeners[i] == 0)
		{
			key_up_listeners[i] = callback;
			return;
		}
}

void keyboard::unregister_key_down(keyboard::keyboard_event_callback callback)
{
	for (size_t i = 0; i < 32; i++)
		if (key_down_listeners[i] == callback)
		{
			key_down_listeners[i] = 0;
			return;
		}
}

void keyboard::unregister_key_up(keyboard::keyboard_event_callback callback)
{
	for (size_t i = 0; i < 32; i++)
		if (key_up_listeners[i] == callback)
		{
			key_up_listeners[i] = 0;
			return;
		}
}