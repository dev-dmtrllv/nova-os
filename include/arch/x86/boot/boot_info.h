#pragma once

#include <lib/stdint.h>

namespace boot
{
    struct boot_info
    {
        uint16_t mem_list_size;
        uint16_t mem_list_addr;
		uint16_t kernel_size;
		uint16_t pci_support;
		uint16_t last_pci_bus;
    } __attribute__((packed));

	static inline boot_info* get_boot_info()
	{
		return reinterpret_cast<boot_info*>(0x3000);
	}
}
