#pragma once

#include <lib/stdint.h>

class vga
{
public:
	enum class color
	{
		BLACK = 0,
		BLUE = 1,
		GREEN = 2,
		CYAN = 3,
		RED = 4,
		MAGENTA = 5,
		BROWN = 6,
		LIGHT_GRAY = 7,
		DARK_GRAY = 8,
		LIGHT_BLUE = 9,
		LIGHT_GREEN = 10,
		LIGHT_CYAN = 11,
		LIGHT_RED = 12,
		LIGHT_MAGENTA = 13,
		YELLOW = 14,
		WHITE = 15
	};

	enum class mode
	{
		TEXT,
		GRAPHICS
	};

	static void set_color(const vga::color color);
	static void set_background(const vga::color color);

	static void write(const char *str);
	static void clear();
	static void clear(const vga::color color);
	// static void set_mode(vga::mode mode);

private:
	enum class ports
	{
		AC_INDEX = 0x3C0,
		AC_WRITE = 0x3C0,
		AC_READ = 0x3C1,
		MISC_WRITE = 0x3C2,
		SEQ_INDEX = 0x3C4,
		SEQ_DATA = 0x3C5,
		DAC_READ_INDEX = 0x3C7,
		DAC_WRITE_INDEX = 0x3C8,
		DAC_DATA = 0x3C9,
		MISC_READ = 0x3CC,
		GC_INDEX = 0x3CE,
		GC_DATA = 0x3CF,
		CRTC_INDEX = 0x3D4,
		CRTC_DATA = 0x3D5,
		INSTAT_READ = 0x3DA
	};

	static vga::color foreground_color;
	static vga::color background_color;
	static vga::mode curr_mode;
	static char colorAttr;

	static void set_cursor(int x, int y);
	static void set_cursor_offset(uint16_t offset);
	static uint16_t get_cursor_offset();
};
