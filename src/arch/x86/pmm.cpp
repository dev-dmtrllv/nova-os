#include <arch/x86/pmm.h>
#include <lib/stdint.h>

extern "C" void load_page_directory(uint32_t page_directory_ptr);
extern "C" void enable_paging();

void pmm::init()
{
	
}
