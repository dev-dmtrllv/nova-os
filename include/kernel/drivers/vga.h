#pragma once

class VGA
{
	public:
		enum class Color
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

		static void setColor(const Color color);
		static void setBackground(const Color color);
		
		static void write(const char* str);

	private:
		static const volatile char* buffer;
		static Color color;
		static Color background;

		static char colorAttr;
		
};
