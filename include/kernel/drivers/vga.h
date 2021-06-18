#pragma once

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

		static void set_color(const vga::color color);
		static void set_background(const vga::color color);
		
		static void write(const char* str);

	private:
		enum class Ports
		{
			INDEX = 0x03c0,
			
		};

		static vga::color foregroundColor;
		static vga::color backgroundColor;

		static char colorAttr;
		
};
