#include <arch/x86/acpi.h>

#include <lib/string.h>

#include <kernel/drivers/vga.h>

#define MAIN_BIOS_AREA_BOTTOM 0xe0000
#define MAIN_BIOS_AREA_TOP 0xfffff

namespace acpi
{
	// root system description pointer
	struct rsdp_desc
	{
		char signature[8];
		uint8_t checksum;
		char oem_id[6];
		uint8_t revision;
		uint32_t rsdt_addr;
	} __attribute__((packed));

	struct rsdp_desc_2
	{
		rsdp_desc first_desc;

		uint32_t length;
		uint64_t x_rsdt_addr;
		uint8_t ext_checksum;
		uint8_t reserved[3];
	} __attribute__((packed));

	rsdt *sys_table;

	rsdp_desc *find_rsdp_table()
	{
		char *rdsp_signature = "RSD PTR ";

		for (size_t i = MAIN_BIOS_AREA_BOTTOM; i < MAIN_BIOS_AREA_TOP; i += 16)
			if (!strncmp(rdsp_signature, reinterpret_cast<char *>(i), 8))
				return reinterpret_cast<rsdp_desc *>(i);

		return nullptr;
	}

	bool check_header(sdt_header *header)
	{
		unsigned char sum = 0;

		for (int i = 0; i < header->length; i++)
			sum += ((char *)header)[i];

		return sum == 0;
	}
};

bool acpi::is_pci_available()
{
	acpi::rsdt *table = acpi::find_table("MCFG");
	if (table != nullptr)
		return true;
	return false;
}

void acpi::init()
{
	rsdp_desc *desc = find_rsdp_table();

	if (desc == nullptr)
	{
		vga::write_line("acpi rsdp desc is nullptr");
	}
	else
	{
		sys_table = reinterpret_cast<rsdt *>(desc->rsdt_addr);
	}
}

acpi::rsdt *acpi::find_table(char signature[4])
{
	size_t entries = (sys_table->header.length - sizeof(sdt_header)) / 4;

	for (int i = 0; i < entries; i++)
	{
		sdt_header *h = reinterpret_cast<sdt_header *>(reinterpret_cast<uint32_t *>(sys_table->data)[i]);

		if (strncmp(h->signature, signature, 4))
			return reinterpret_cast<rsdt *>(h);
	}
	return nullptr;
}
