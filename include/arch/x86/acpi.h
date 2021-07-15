#pragma once

#include <lib/stdint.h>

namespace acpi
{
	// system description table header
	struct sdt_header
	{
		char signature[4];
		uint32_t length;  // total size of the table (including header)
		uint8_t revision; //
		uint8_t checksum;
		char oem_id[6];
		char oem_table_id[8];
		uint32_t oem_revision;
		uint32_t creator_id;
		uint32_t creator_revision;
	};

	struct rsdt
	{
		sdt_header header;
		void *data;
	};

	bool is_pci_available();
	void init();
	rsdt *find_table(char signature[4]);
};
