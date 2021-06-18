#include <kernel/drivers/vga.h>

// const volatile char* VGA::buffer = (volatile char*);

VGA::Color VGA::color = VGA::Color::WHITE;
VGA::Color VGA::background = VGA::Color::BLACK;

char VGA::colorAttr = 0x0f;

void VGA::setColor(const VGA::Color color)
{
	VGA::color = color;
	VGA::colorAttr = static_cast<char>(VGA::color) | static_cast<char>(VGA::background) << 4;
}

void VGA::setBackground(const VGA::Color bgColor)
{
	VGA::background = bgColor;
	VGA::colorAttr = static_cast<char>(VGA::color) | static_cast<char>(VGA::background) << 4;
}

void VGA::write(const char* str)
{
	volatile char* ptr = (volatile char*)0xb8000;
	while(*str != 0)
    {
        *ptr++ = *str++;
        *ptr++ = VGA::colorAttr;
    }
}
