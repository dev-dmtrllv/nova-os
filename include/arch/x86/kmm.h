#pragma once

#include <lib/stdint.h>

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

		constexpr uint32_t FRAME_BLOCK_SIZE = 4096; // 4Kb

		uint64_t align(uint64_t num, uint64_t alignment);
		uint64_t align(uint64_t num);

		mem_info_t memory_info[32];

		uint32_t *bitmap = nullptr;

		void set_bitmap(size_t frame_index, bool free);
	};

	void init();
};
