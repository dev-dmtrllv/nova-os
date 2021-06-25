#include <kernel/drivers/vga.h>
#include <kernel/io.h>
#include <lib/string.h>

#define VGA_TXT_WIDTH 80
#define VGA_TXT_HEIGHT 25

vga::color vga::foreground_color = vga::color::WHITE;
vga::color vga::background_color = vga::color::BLACK;

unsigned char vga_buffer[VGA_TXT_WIDTH * 2];

char vga::colorAttr = 0x0f;

void vga::set_color(const vga::color color)
{
	vga::foreground_color = color;
	vga::colorAttr = static_cast<char>(vga::foreground_color) | static_cast<char>(vga::background_color) << 4;
}

void vga::set_background(const vga::color bgColor)
{
	vga::background_color = bgColor;
	vga::colorAttr = static_cast<char>(vga::foreground_color) | static_cast<char>(vga::background_color) << 4;
}

void vga::write(const char *str)
{
	uint16_t offset = get_cursor_offset();
	volatile char *ptr = (volatile char *)(0xb8000 + (offset * 2));

	while (*str != 0)
	{
		if (*str == '\n')
		{
			offset = ((offset / VGA_TXT_WIDTH) + 1) * VGA_TXT_WIDTH;
			ptr += 2;
			str++;
		}
		else if (*str == '\t')
		{
			str++;

			uint16_t lp = offset / VGA_TXT_WIDTH;
			uint16_t np = offset + 4 / VGA_TXT_WIDTH;

			if (lp != np) // skip to new line
			{
				ptr++;
				offset = ((offset / VGA_TXT_WIDTH) + 1) * VGA_TXT_WIDTH;
				break;
			}

			for (uint8_t i = 0; i < 4; i++)
			{
				offset++;
				*ptr++ = ' ';
				*ptr++ = vga::colorAttr;

				if (offset >= (VGA_TXT_WIDTH * VGA_TXT_HEIGHT))
				{
					for (size_t i = 0; i < VGA_TXT_HEIGHT; i++)
						vga::copy_row(i + 1, i);
					offset = (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1));
					ptr = (volatile char *)0xb8000 + (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1) * 2);
				}
			}
		}
		else
		{
			offset++;
			*ptr++ = *str++;
			*ptr++ = vga::colorAttr;
		}

		if (offset >= (VGA_TXT_WIDTH * VGA_TXT_HEIGHT))
		{
			for (size_t i = 0; i < VGA_TXT_HEIGHT; i++)
				vga::copy_row(i + 1, i);
			offset = (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1));
			ptr = (volatile char *)0xb8000 + (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1) * 2);
		}
	}

	set_cursor_offset(offset);
}

void vga::copy_row(uint32_t from, uint32_t to)
{
	memcpy(vga_buffer, (void *)(0xb8000) + (from * VGA_TXT_WIDTH * 2), VGA_TXT_WIDTH * 2);
	memcpy((void *)(0xb8000) + (to * VGA_TXT_WIDTH * 2), vga_buffer, VGA_TXT_WIDTH * 2);
}

void vga::clear()
{
	vga::clear(vga::color::BLACK);
}

void vga::clear(const vga::color color)
{
	volatile char *ptr = (volatile char *)0xb8000;
	char c = 0x0 | static_cast<char>(color) << 4;
	for (uint16_t i = 0; i < VGA_TXT_WIDTH * VGA_TXT_HEIGHT; i++)
	{
		*ptr++ = ' ';
		*ptr++ = c;
	}
	vga::set_cursor(0, 0);
}

void vga::set_cursor(int x, int y)
{
	vga::set_cursor_offset(y * VGA_TXT_WIDTH + x);
}

void vga::set_cursor_offset(uint16_t offset)
{
	io::outb(0x3d4, 0x0f);
	io::outb(0x3d5, (uint8_t)(offset & 0xff));
	io::outb(0x3d4, 0x0e);
	io::outb(0x3d5, (uint8_t)((offset >> 8) & 0xff));
}

uint16_t vga::get_cursor_offset()
{
	uint16_t pos = 0;
	io::outb(0x3D4, 0x0F);
	pos |= io::inb(0x3D5);
	io::outb(0x3D4, 0x0E);
	pos |= ((uint16_t)io::inb(0x3D5)) << 8;
	return pos;
}
