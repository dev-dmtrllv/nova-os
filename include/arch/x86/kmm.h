#pragma once

#include <lib/stdint.h>

namespace kmm
{
	namespace
	{
		struct bios_mem_info
		{
			uint32_t base_l;
			uint32_t base_h;
			uint32_t size_l;
			uint32_t size_h;
			uint32_t type;
			uint32_t acpi;
		} __attribute__((packed));
	};

	void init();
};
