#pragma once

#include <lib/stdint.h>

namespace vga
{
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

// 	enum class mode
// 	{
// 		TEXT,
// 		GRAPHICS
// 	};

// 	void copy_row(uint32_t from, uint32_t to);

// 	void set_color(const vga::color color);
// 	void set_background(const vga::color color);

// 	void write(const char *str);
// 	void write_line(const char *str);

// 	template <typename ...T>
// 	void write(const char *str, T&... args)
// 	{
// 		vga::write(str);
// 		vga::write(args...);
// 	}

// 	template <typename ...T>
// 	void write_line(const char *str, T&... args)
// 	{
// 		vga::write(str);
// 		vga::write(args...);
// 		vga::write("\n");
// 	}

// 	void clear();
// 	void clear(const vga::color color);
// 	// void set_mode(vga::mode mode);

// private:
// 	enum class ports
// 	{
// 		AC_INDEX = 0x3C0,
// 		AC_WRITE = 0x3C0,
// 		AC_READ = 0x3C1,
// 		MISC_WRITE = 0x3C2,
// 		SEQ_INDEX = 0x3C4,
// 		SEQ_DATA = 0x3C5,
// 		DAC_READ_INDEX = 0x3C7,
// 		DAC_WRITE_INDEX = 0x3C8,
// 		DAC_DATA = 0x3C9,
// 		MISC_READ = 0x3CC,
// 		GC_INDEX = 0x3CE,
// 		GC_DATA = 0x3CF,
// 		CRTC_INDEX = 0x3D4,
// 		CRTC_DATA = 0x3D5,
// 		INSTAT_READ = 0x3DA
// 	};

// 	vga::color foreground_color;
// 	vga::color background_color;
// 	vga::mode curr_mode;
// 	char colorAttr;

// 	void set_cursor(int x, int y);
// 	void set_cursor_offset(uint16_t offset);
// 	uint16_t get_cursor_offset();
	void write(const char* str);
};
