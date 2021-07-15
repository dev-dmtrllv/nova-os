#include <arch/x86/kinit/paging.h>

#include <lib/cstdlib.h>
#include <lib/string.h>

namespace paging
{
	uint32_t kernel_directory[PAGE_ENTRIES] __attribute__((aligned(0x1000)));

	uint32_t low_mem_entries[PAGE_ENTRIES] __attribute__((aligned(0x1000)));
}

void paging::init()
{
	// clear the kernel_directory table
	memset(kernel_directory, 0x00000002, sizeof(uint32_t) * PAGE_ENTRIES);
	
	low_mem_entries[IDT_GDT_PAGE] = 0x0 | 3;
	low_mem_entries[KERNEL_DIR_PAGE] = reinterpret_cast<uint32_t>(kernel_directory) | 3;
	low_mem_entries[LOW_MEM_PAGE] = reinterpret_cast<uint32_t>(low_mem_entries) | 3;
	
	for(size_t i = 0; i < VIDEO_MEMORY_PAGE_SIZE; i++)
		low_mem_entries[VIDEO_MEMORY_PAGE + i] = (PHYC_VIDEO_ADDR + (i * PAGE_SIZE));

	kernel_directory[KERNEL_DIR_INDEX] = reinterpret_cast<uint32_t>(low_mem_entries) | 3;
}
