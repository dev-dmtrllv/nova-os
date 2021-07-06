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

	void init();
	void init(vga::color foreground, vga::color background);


	void set_color(const vga::color color);
	void set_background(const vga::color color);

	void write_at(const char* str, size_t x, size_t y);

	void write(const char *str);
	void write_line(const char *str);

	void write(const char* str, vga::color fg,	vga::color bg);
	void write_line(const char* str, vga::color fg, vga::color bg);

	template <typename... T>
	void write(const char *str, T &...args)
	{
		vga::write(str);
		vga::write(args...);
	}

	template <typename... T>
	void write_line(const char *str, T &...args)
	{
		vga::write(str);
		vga::write(args...);
		vga::write("\n");
	}

	void clear();
	void clear(const vga::color color);
};