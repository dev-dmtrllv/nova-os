#include <arch/x86/kmm.h>

#include <kernel/drivers/vga.h>
#include <lib/string.h>

void kmm::init()
{
	size_t max_bios_mem_info_size = (0x2000 - 0x1500) / sizeof(bios_mem_info_t);
	size_t size = 0;

	uint64_t min_ram = 0xffffffffffffffff;
	uint64_t max_ram = 0;

	char buf1[16];
	char buf2[16];
	char buf3[16];

	bios_mem_info_t *bios_mem_info_ptr = reinterpret_cast<bios_mem_info_t *>(0x1500);

	vga::write("\n");
	vga::write("Memory map:\n");
	vga::write("from:   \t\tto: \t\t\tsize:\n");

	while (size < max_bios_mem_info_size)
	{
		mem_info_t *info = &memory_info[size];
		info->base = ((bios_mem_info_ptr[size].base_h << 32) | bios_mem_info_ptr[size].base_l);
		info->size = ((bios_mem_info_ptr[size].size_h << 32) | bios_mem_info_ptr[size].size_l);
		info->reserved = bios_mem_info_ptr[size].type != 1;

		if ((info->base == 0) && (info->size == 0))
			break;

		vga::write(ltoa(info->base, buf1, 16));

		for (size_t i = 0; i < (16 - strlen(buf1)); i++)
			vga::write(" ");

		vga::write(ltoa(info->base + info->size, buf1, 16));

		for (size_t i = 0; i < (16 - strlen(buf1)); i++)
			vga::write(" ");

		vga::write(ltoa(info->size, buf1, 16));

		for (size_t i = 0; i < (16 - strlen(buf1)); i++)
			vga::write(" ");

		vga::write(info->reserved ? "reserved\n" : "free\n");

		if (!info->reserved)
		{
			if (info->base < min_ram)
				min_ram = info->base;
			if (info->base + info->size > max_ram)
				max_ram = info->base + info->size;
		}

		size++;
	}

	utoa(size, buf1, 10);
	vga::write_line("list size: ", buf1);

	utoa(max_bios_mem_info_size, buf1, 10);
	vga::write_line("max list size: ", buf1);

	utoa(min_ram, buf1, 16);
	utoa(max_ram, buf2, 16);
	vga::write_line("ram: ", buf1, " - ", buf2);

	uint64_t ram_size = max_ram - min_ram;
	uint32_t frames = align(ram_size, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
	uint32_t integers = align(frames, 32) / 32;
	uint32_t integers_size = integers * sizeof(uint32_t);

	bitmap = reinterpret_cast<uint32_t *>(max_ram - integers_size);

	utoa((uint32_t)bitmap, buf1, 16);
	utoa(integers_size, buf2, 16);
	vga::write_line("bitmap address: ", buf1, "\t\tsize: ", buf2);

	memset(bitmap, 0, integers_size);

	size_t total_size = size;

	for (size_t i = 0; i < size; i++)
	{
		uint64_t lowest = 0xffffffffffffffff;
		for (size_t j = i + 1; j < size; j++)
			if(memory_info[j].base < lowest)
				lowest = memory_info[j].base;
		
		uint64_t top = memory_info[i].base + memory_info[i].size;
		if(top != lowest)
		{
			mem_info_t *info = &memory_info[total_size++];
			info->base = top;
			info->size = lowest - top;
			info->reserved = true;
		}
	}

	for (size_t i = 0; i < total_size; i++)
	{
		if (memory_info[i].reserved)
		{
			uint32_t base_frame_index = align(memory_info[i].base, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
			uint32_t limit_frame_index = align(memory_info[i].base + memory_info[i].size, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
			for (base_frame_index; base_frame_index <= limit_frame_index; base_frame_index++)
				set_bitmap(base_frame_index, false);
		}
	}
}

namespace kmm
{
	namespace
	{
		uint64_t align(uint64_t num, uint64_t alignment)
		{
			if (num % alignment == 0)
				return num;
			return ((num / alignment) + 1) * alignment;
		}

		uint64_t align(uint64_t num)
		{
			return align(num, FRAME_BLOCK_SIZE);
		}

		void set_bitmap(size_t frame_index, bool free)
		{
			size_t i = frame_index / 32;
			size_t bitmap_index = frame_index % 32;

			if (free)
				bitmap[i] &= ~(1 << bitmap_index);
			else
				bitmap[i] |= 1 << bitmap_index;
		}
	};
}
