#include <arch/x86/kinit/kmm.h>
#include <arch/x86/boot/bios_info.h>
#include <kernel/drivers/vga.h>

#include <lib/string.h>

#define FRAME_BLOCK_SIZE 4096

namespace kmm
{
	namespace
	{

		struct bios_mem_info_t
		{
			uint32_t base_l;
			uint32_t base_h;
			uint32_t size_l;
			uint32_t size_h;
			uint32_t type;
			uint32_t acpi;
		} __attribute__((packed));

		struct mem_info_t
		{
			uint64_t base;
			uint64_t size;
			bool reserved;
		} __attribute__((packed));

		mem_info_t memory_info[32];

		uint32_t *bitmap = nullptr;

		uint64_t align(uint64_t num, uint64_t alignment)
		{
			if (num % alignment == 0)
				return num;
			return ((num / alignment) + 1) * alignment;
		}

		// uint64_t align(uint64_t num)
		// {
		// 	return align(num, FRAME_BLOCK_SIZE);
		// }

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

void kmm::init()
{
	boot::bios_info* bios_info = reinterpret_cast<boot::bios_info*>(0x3000);
	
	size_t size = 0;

	uint64_t min_ram = 0xffffffffffffffff;
	uint64_t max_ram = 0;

	char buf1[16];
	char buf2[16];

	bios_mem_info_t *bios_mem_info_ptr = reinterpret_cast<bios_mem_info_t *>(bios_info->mem_list_addr);

	vga::write("Memory map:\n");
	vga::write("from:   \t\tto: \t\t\tsize:\n");

	while (size < bios_info->mem_list_size)
	{
		mem_info_t *info = &memory_info[size];
		info->base = ((static_cast<uint64_t>(bios_mem_info_ptr[size].base_h) << 32) | bios_mem_info_ptr[size].base_l);
		info->size = ((static_cast<uint64_t>(bios_mem_info_ptr[size].size_h) << 32) | bios_mem_info_ptr[size].size_l);
		info->reserved = bios_mem_info_ptr[size].type != 1;

		if ((info->base == 0) && (info->size == 0))
			break;

		vga::write(ltoa(info->base, buf1, 16));

		for (size_t i = 0; i < (16 - strlen(buf1)); i++)
			vga::write(" ");

		vga::write(ltoa(info->base + info->size - 1, buf1, 16));

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

	size_t total_size = size;


	vga::write_line("\nchecking for memory gaps...");
	for (size_t i = 0; i < size - 1; i++)
	{

		uint64_t b = memory_info[i].base;
		uint64_t t = memory_info[i].base + memory_info[i].size;

		int gap_dir = 0; // 0 means no gap, -1 means gap downwards, 1 means gap upwards

		uint64_t gap = 0xffffffffffffffff;

		for (size_t j = i + 1; j < size; j++)
		{
			if (i != j)
			{
				uint64_t b2 = memory_info[j].base;
				uint64_t t2 = memory_info[j].base + memory_info[j].size;
				if (t == b2 || t2 == b)
				{
					gap_dir = 0;
					gap = 0;
					break;
				}

				if (t < b2 && t < max_ram) // the top is lower than the others base (so the gap is above us)
				{
					uint64_t g = b2 - t;
					if (g < gap)
						gap = g;
					gap_dir = 1;
				}
				else if (b > t2 && b < max_ram) // the base is higher than the others top (so the gap is below us)
				{
					uint64_t g = b - t2;
					if (g < gap)
						gap = g;
					gap_dir = -1;
				}
			}
		}

		if (gap_dir != 0)
		{
			mem_info_t *info = &memory_info[total_size++];
			info->size = gap;
			info->base = gap_dir == -1 ? b - gap : t;
			info->reserved = true;

			utoa(info->base, buf1, 16);
			utoa(info->size, buf2, 16);
			vga::write_line("found gap: ", buf1, "\tsize: ", buf2);
		}
	}

	vga::write_line("");

	utoa(min_ram, buf1, 16);
	utoa(max_ram, buf2, 16);
	vga::write_line("available ram: ", buf1, " - ", buf2);

	uint64_t ram_size = max_ram - min_ram;
	uint32_t frames = align(ram_size, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
	uint32_t integers = align(frames, 32) / 32;
	uint32_t integers_size = integers * sizeof(uint32_t);

	bitmap = reinterpret_cast<uint32_t *>(max_ram - integers_size);

	utoa((uint32_t)bitmap, buf1, 16);
	utoa(integers_size, buf2, 16);
	vga::write_line("bitmap address: ", buf1, "\tsize: ", buf2);

	memset(bitmap, 0, integers_size);

	vga::write_line("\ninitializing bitmaps...");

	for (size_t i = 0; i < total_size; i++)
	{
		if (memory_info[i].reserved)
		{
			ltoa(memory_info[i].base, buf1, 16);
			utoa(memory_info[i].size, buf2, 16);
			
			vga::write("set reserved bitmap for: ", buf1);
			for (size_t i = 0; i < (16 - strlen(buf1)); i++)
				vga::write(" ");
			vga::write_line("size: ", buf2);

			uint32_t base_frame_index = align(memory_info[i].base, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
			uint32_t limit_frame_index = align(memory_info[i].base + memory_info[i].size, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
			for (; base_frame_index <= limit_frame_index; base_frame_index++)
				set_bitmap(base_frame_index, false);
		}
	}
}