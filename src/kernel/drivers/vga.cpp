#include <kernel/drivers/vga.h>

vga::color vga::foregroundColor = vga::color::WHITE;
vga::color vga::backgroundColor = vga::color::BLACK;

char vga::colorAttr = 0x0f;

void vga::set_color(const vga::color color)
{
	vga::foregroundColor = color;
	vga::colorAttr = static_cast<char>(vga::foregroundColor) | static_cast<char>(vga::backgroundColor) << 4;
}

void vga::set_background(const vga::color bgColor)
{
	vga::backgroundColor = bgColor;
	vga::colorAttr = static_cast<char>(vga::foregroundColor) | static_cast<char>(vga::backgroundColor) << 4;
}

void vga::write(const char* str)
{
	volatile char* ptr = (volatile char*)0xb8000;
	while(*str != 0)
    {
        *ptr++ = *str++;
        *ptr++ = vga::colorAttr;
    }
}
