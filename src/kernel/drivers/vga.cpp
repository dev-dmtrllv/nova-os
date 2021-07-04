#include <kernel/drivers/vga.h>
#include <arch/x86/io.h>
#include <lib/string.h>

#define VGA_TXT_WIDTH 80
#define VGA_TXT_HEIGHT 25

namespace vga
{
	namespace
	{
		vga::color background_color;
		vga::color foreground_color;

		volatile char color_attr;

		char vga_buffer[VGA_TXT_WIDTH * 2];

		uint16_t get_cursor_offset()
		{
			uint16_t pos = 0;
			io::outb(0x3D4, 0x0F);
			pos |= io::inb(0x3D5);
			io::outb(0x3D4, 0x0E);
			pos |= ((uint16_t)io::inb(0x3D5)) << 8;
			return pos;
		}

		void set_cursor_offset(uint16_t offset)
		{
			io::outb(0x3d4, 0x0f);
			io::outb(0x3d5, (uint8_t)(offset & 0xff));
			io::outb(0x3d4, 0x0e);
			io::outb(0x3d5, (uint8_t)((offset >> 8) & 0xff));
		}

		void set_cursor(int x, int y)
		{
			vga::set_cursor_offset(y * VGA_TXT_WIDTH + x);
		}

		void copy_row(uint32_t from, uint32_t to)
		{
			memcpy(vga_buffer, reinterpret_cast<void *>(0xb8000 + (from * VGA_TXT_WIDTH * 2)), VGA_TXT_WIDTH * 2);
			memcpy(reinterpret_cast<void *>(0xb8000 + (to * VGA_TXT_WIDTH * 2)), vga_buffer, VGA_TXT_WIDTH * 2);
		}

		void scroll()
		{
			for (size_t i = 0; i < VGA_TXT_HEIGHT; i++)
				vga::copy_row(i + 1, i);
		}
	}
};

void vga::init()
{
	vga::set_background(vga::color::BLACK);
	vga::set_color(vga::color::LIGHT_GRAY);
}

void vga::init(vga::color foreground, vga::color background)
{
	vga::set_background(background);
	vga::set_color(foreground);
}

void vga::write(const char *str)
{
	uint16_t offset = get_cursor_offset();
	volatile char *ptr = reinterpret_cast<volatile char *>(0xb8000 + (offset * 2));

	while (*str != 0)
	{
		if (*str == '\n')
		{
			str++;
			offset = ((offset / VGA_TXT_WIDTH) + 1) * VGA_TXT_WIDTH;
			if (offset >= (VGA_TXT_HEIGHT * VGA_TXT_WIDTH))
			{
				offset = ((VGA_TXT_HEIGHT - 1) * VGA_TXT_WIDTH);
				vga::scroll();
			}
			ptr = reinterpret_cast<volatile char *>(0xb8000 + (offset * 2));
		}
		else if (*str == '\t')
		{
			str++;

			uint16_t lp = offset / VGA_TXT_WIDTH;

			uint16_t np = (offset + (4 - (offset % 4))) / VGA_TXT_WIDTH;

			if (lp != np)
			{
				offset = 0;
				ptr = reinterpret_cast<volatile char *>(0xb8000);
			}
			else
			{
				uint32_t l = 0;
				uint32_t line_offset = offset % VGA_TXT_WIDTH;
				if (line_offset != 0)
				{
					uint32_t rem = line_offset % 4;
					if (rem == 0)
						l = 4;
					else
						l = 4 - rem;
				}

				for (uint8_t i = 0; i < l; i++)
				{
					offset++;
					*ptr++ = ' ';
					*ptr++ = vga::color_attr;

					if (offset >= (VGA_TXT_WIDTH * VGA_TXT_HEIGHT))
					{
						vga::scroll();
						offset = (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1));
						ptr = (volatile char *)0xb8000 + (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1) * 2);
					}
				}
			}
		}
		else
		{
			offset++;
			*ptr++ = *str++;
			*ptr++ = vga::color_attr;
		}

		if (offset >= (VGA_TXT_WIDTH * VGA_TXT_HEIGHT))
		{
			vga::scroll();
			offset = (VGA_TXT_WIDTH * (VGA_TXT_HEIGHT - 1));
			ptr = (volatile char *)0xb8000 + (offset * 2);
		}
	}
	set_cursor_offset(offset);
}

void vga::set_color(const vga::color color)
{
	foreground_color = color;
	vga::color_attr = static_cast<char>(vga::foreground_color) | static_cast<char>(vga::background_color) << 4;
}

void vga::set_background(const vga::color background_colorColor)
{
	vga::background_color = background_colorColor;
	vga::color_attr = static_cast<char>(vga::foreground_color) | static_cast<char>(vga::background_color) << 4;
}

void vga::write_line(const char *str)
{
	vga::write(str, "\n");
}

void vga::clear()
{
	vga::clear(vga::color::BLACK);
}

void vga::clear(const vga::color color)
{
	volatile char *ptr = reinterpret_cast<volatile char *>(0xb8000);
	const char c = 0x0 | (static_cast<char>(color) << 4);
	for (uint16_t i = 0; i < VGA_TXT_WIDTH * VGA_TXT_HEIGHT; i++)
	{
		*ptr++ = ' ';
		*ptr++ = c;
	}
	vga::set_cursor(0, 0);
}