#include <arch/x86/kinit/kmm.h>
#include <arch/x86/boot/bios_info.h>
#include <kernel/drivers/vga.h>

#include <lib/string.h>

#define PAGE_SIZE 0x1000
#define PAGE_ENTRIES 1024

#define FRAME_BLOCK_SIZE 0x1000
#define MAX_RAM 0x100000000
#define FRAME_INFO_LIST_SIZE 1024

extern uint32_t kinit_end;

extern "C" void load_page_directory(uint32_t page_directory_ptr);
extern "C" void enable_paging();

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

		struct frame_info_t
		{
			uint32_t address;
			size_t size;
		} __attribute__((packed));

		mem_info_t memory_info_list[64];

		frame_info_t frame_info_list[FRAME_INFO_LIST_SIZE];

		uint32_t page_directory[1024] __attribute__((aligned(0x1000)));
		uint32_t page_table[1024] __attribute__((aligned(0x1000)));

		uint32_t *bitmap = nullptr;
		uint32_t bitmap_size;

		uint64_t align(uint64_t num, uint64_t alignment)
		{
			if (num % alignment == 0)
				return num;
			return ((num / alignment) + 1) * alignment;
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

void kmm::init()
{
	boot::bios_info *bios_info = reinterpret_cast<boot::bios_info *>(0x3000);

	size_t size = 0;

	uint64_t min_ram = 0xffffffffffffffff;
	uint64_t max_ram = 0;

	char buf1[16];
	char buf2[16];

	bios_mem_info_t *bios_mem_info_ptr = reinterpret_cast<bios_mem_info_t *>(bios_info->mem_list_addr);

	vga::write("Memory map:\n");
	vga::write("from:   \t\tto: \t\t\tsize:\n");

	for (size_t i = 0; i < bios_info->mem_list_size; i++)
	{
		mem_info_t *info = &memory_info_list[size];
		info->base = ((static_cast<uint64_t>(bios_mem_info_ptr[i].base_h) << 32) | bios_mem_info_ptr[i].base_l);
		info->size = ((static_cast<uint64_t>(bios_mem_info_ptr[i].size_h) << 32) | bios_mem_info_ptr[i].size_l);
		info->reserved = bios_mem_info_ptr[i].type != 1;

		if (info->base >= MAX_RAM) // first check if we are within the 4GB bounds
			continue;
		else if (info->base + info->size - 1 >= MAX_RAM) // else if the top lays above the MAX_RAM we will reset the size to never go beyond the MAX_RAM
			info->size = MAX_RAM - info->base;

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

		uint64_t b = memory_info_list[i].base;
		uint64_t t = memory_info_list[i].base + memory_info_list[i].size;

		int gap_dir = 0; // 0 means no gap, -1 means gap downwards, 1 means gap upwards

		uint64_t gap = 0xffffffffffffffff;

		for (size_t j = i + 1; j < size; j++)
		{
			if (i != j)
			{
				uint64_t b2 = memory_info_list[j].base;
				uint64_t t2 = memory_info_list[j].base + memory_info_list[j].size;
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
			mem_info_t *info = &memory_info_list[total_size++];
			info->size = gap;
			info->base = gap_dir == -1 ? b - gap : t;
			info->reserved = true;

			utoa(info->base, buf1, 16);
			utoa(info->size, buf2, 16);
			vga::write_line("found gap: ", buf1, "\tsize: ", buf2);
		}
	}

	utoa(min_ram, buf1, 16);
	utoa(max_ram, buf2, 16);
	vga::write_line("available ram: ", buf1, " - ", buf2);

	uint64_t ram_size = max_ram - min_ram;
	uint32_t frames = align(ram_size, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
	uint32_t integers = align(frames, 32) / 32;

	bitmap_size = integers * sizeof(uint32_t);
	bitmap = reinterpret_cast<uint32_t *>(max_ram - bitmap_size);

	utoa((uint32_t)bitmap, buf1, 16);
	utoa(bitmap_size, buf2, 16);
	vga::write_line("bitmap address: ", buf1, "\tsize: ", buf2);

	memset(bitmap, 0, bitmap_size);

	vga::write_line("\ninitializing bitmaps...");

	for (size_t i = 0; i < total_size; i++)
	{
		if (memory_info_list[i].reserved)
		{
			if (memory_info_list[i].base < max_ram)
			{
				uint32_t base_frame_index = align(memory_info_list[i].base, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
				uint32_t limit_frame_index = align(memory_info_list[i].base + memory_info_list[i].size, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;

				utoa(memory_info_list[i].base, buf1, 16);
				utoa(memory_info_list[i].base + memory_info_list[i].size, buf2, 16);

				vga::write("set reserved bitmap for: ", buf1);
				for (size_t i = 0; i < (16 - strlen(buf1)); i++)
					vga::write(" ");
				vga::write_line("limit: ", buf2);

				for (; base_frame_index <= limit_frame_index; base_frame_index++)
					set_bitmap(base_frame_index, false);
			}
		}
	}

	// set the first 4 frames reserved (this is for the IDT, GDT, BIOS_KERNEL_INFO_AREA)
	for (size_t i = 0; i < 4; i++)
		set_bitmap(i, false);

	// set all the kinit frames reserved
	uint32_t kinit_start = align(0x10000, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
	const uint32_t kinit_end = align(reinterpret_cast<uint32_t>(&kinit_end), FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
	for (; kinit_start <= kinit_end; kinit_start++)
		set_bitmap(kinit_start, false);

	// do the same for the kernel (since the kernel has no functionality yet we can omit this)
	// ...

	// also set the bitmap frames reserved (reserve everything from bitmap address up till max ram)
	uint32_t bitmap_index = align(reinterpret_cast<uint32_t>(bitmap), FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
	uint32_t top_index = align(static_cast<uint32_t>(max_ram), FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;

	for (; bitmap_index < top_index; bitmap_index++)
		set_bitmap(bitmap_index, false);

	// everything is set no just zero out the frame_info_list and setup and empty page directory
	memset(frame_info_list, 0, sizeof(frame_info_t) * FRAME_INFO_LIST_SIZE);
	memset(page_directory, 0x00000002, sizeof(uint32_t) * PAGE_ENTRIES);

	for (size_t i = 0; i < PAGE_ENTRIES; i++)
		page_table[i] = (i * PAGE_SIZE) | 3;

	// put the page table in as the first entry in the page directory
	page_directory[0] = reinterpret_cast<uint32_t>(page_table) | 3;

	load_page_directory(reinterpret_cast<uint32_t>(page_directory));
	enable_paging();
}

void *kmm::alloc_frames(size_t number_of_frames)
{
	size_t free_frames = 0;
	uint32_t frame_index = 0;
	uint32_t frame_bit = 0;

	char buf[8];

	for (size_t i = 0; i < bitmap_size; i++)
	{
		uint32_t n = bitmap[i];

		// loop through each bit till we find a row of free frames
		for (size_t j = 0; j < 32; j++)
		{
			if (((n >> j) & 1) == 0) // bit j is free
			{
				if (free_frames == 0)
				{
					frame_index = i;
					frame_bit = j;
				}

				free_frames++;

				if (free_frames == number_of_frames)
				{
					uint32_t frame_start_index = (frame_index * 32) + frame_bit;
					for (size_t k = 0; k < number_of_frames; k++)
						kmm::set_bitmap(frame_start_index + k, false);

					// find free entry in the frame_info_list
					for (size_t k = 0; k < FRAME_INFO_LIST_SIZE; k++)
					{
						if (frame_info_list[k].address == 0 && frame_info_list[k].address == 0)
						{
							frame_info_list[k].address = frame_start_index * FRAME_BLOCK_SIZE;
							frame_info_list[k].size = number_of_frames;
							break;
						}
					}
					return reinterpret_cast<void *>(frame_start_index * FRAME_BLOCK_SIZE);
				}
			}
			else
			{
				free_frames = 0;
			}
		}
	}

	return nullptr;
}

void kmm::free_frames(void *address)
{
	uint32_t adr = reinterpret_cast<uint32_t>(address);
	size_t size = 0;
	for (size_t i = 0; i < FRAME_INFO_LIST_SIZE; i++)
	{
		if (frame_info_list[i].address == adr)
		{
			size = frame_info_list[i].size;
			frame_info_list[i].address = 0;
			frame_info_list[i].size = 0;
			break;
		}
	}
	if (size > 0)
	{
		uint32_t base_frame_index = align(adr, FRAME_BLOCK_SIZE) / FRAME_BLOCK_SIZE;
		for (size_t j = 0; j < size; j++)
			kmm::set_bitmap(base_frame_index + j, true);
	}
}