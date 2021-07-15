#pragma once

#define KERNEL_DIR_INDEX 768 // 3 GiB

#define PAGE_SIZE 0x1000

#define PAGE_ENTRIES 1024

#define IDT_GDT_PAGE 0
#define KERNEL_DIR_PAGE 1
#define LOW_MEM_PAGE 2
#define VIDEO_MEMORY_PAGE 3

#define VIDEO_MEMORY_PAGE_SIZE 32

#define PHYC_VIDEO_ADDR 0xa0000

#define VIR_VIDEO_ADDR ((KERNEL_DIR_INDEX + VIDEO_MEMORY_PAGE) * 0x1000)

namespace pmm
{
	void init();
}
