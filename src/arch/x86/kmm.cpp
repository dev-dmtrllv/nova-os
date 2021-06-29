#include <arch/x86/kmm.h>

#include <kernel/drivers/vga.h>
#include <lib/string.h>

void kmm::init()
{
	size_t max_bios_mem_info_size = (0x2000 - 0x1500) / sizeof(bios_mem_info);
	size_t size = 0;

	uint64_t min_ram = 0xffffffffffffffff;
	uint64_t max_ram = 0;

	char buf[16];

	bios_mem_info *bios_mem_info_ptr = reinterpret_cast<bios_mem_info *>(0x1500);

	vga::write("\n");
	vga::write("Memory map:\n");
	vga::write("from:   \t\tto: \t\t\tsize:\n");

	while (size < max_bios_mem_info_size)
	{
		uint64_t info_base = ((bios_mem_info_ptr[size].base_h << 32) | bios_mem_info_ptr[size].base_l);
		uint64_t info_size = ((bios_mem_info_ptr[size].size_h << 32) | bios_mem_info_ptr[size].size_l);

		if ((info_base == 0) && (info_size == 0))
			break;

		vga::write(ltoa(info_base, buf, 16));

		for (size_t i = 0; i < (16 - strlen(buf)); i++)
			vga::write(" ");

		vga::write(ltoa(info_base + info_size, buf, 16));

		for (size_t i = 0; i < (16 - strlen(buf)); i++)
			vga::write(" ");

		vga::write(ltoa(info_size, buf, 16));

		for (size_t i = 0; i < (16 - strlen(buf)); i++)
			vga::write(" ");

		vga::write(bios_mem_info_ptr[size].type == 1 ? "free\n" : "reserved\n");

		if (bios_mem_info_ptr[size].type == 1)
		{
			if (info_base < min_ram)
				min_ram = info_base;
			if (info_base + info_size > max_ram)
				max_ram = info_base + info_size;
		}
		size++;
	}

	vga::write("list size: ");
	vga::write(utoa(size, buf, 10));
	vga::write("\n");
	vga::write("max list size: ");
	vga::write(utoa(max_bios_mem_info_size, buf, 10));
	vga::write("\n");

	vga::write("ram: ");
	vga::write(utoa(min_ram, buf, 16));
	vga::write(" - ");
	vga::write(utoa(max_ram, buf, 16));
	vga::write("\n");

	// find gaps
}
