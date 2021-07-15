#include <arch/x86/pci.h>
#include <arch/x86/io.h>
#include <arch/x86/acpi.h>

#include <arch/x86/boot/boot_info.h>

#include <kernel/drivers/vga.h>

#include <lib/stdint.h>
#include <lib/string.h>

namespace pci
{
	// #define CONFIG_ADDR 0xCF8
	// #define CONFIG_DATA 0x

	struct common_header
	{
		uint16_t vendor_id;
		uint16_t device_id;
		uint16_t command;			   // provides control over a device. (all devices support writing 0 to this register, it will disconnect the device from the pci bus only allowing configuration access)
		uint16_t status;			   // record pci bus events
		uint8_t revision_id;		   // id for particular device
		uint8_t programming_interface; // specifies if a register-level programming interface is available
		uint8_t sub_class;			   // specific function
		uint8_t class_code;			   // type of function
		uint8_t cache_line_size;	   // specifies the system cache line size in 32 bit units
		uint8_t latency_timer;		   // latency timer in units of PCI bus clocks
		uint8_t header_type;		   // one of the following header types (if the highest bit is 1 the device has multiple functions) [0 = standard, 1 = pci-pci bridge, 2 = cardbus bridge]
		uint8_t bist;				   // built in self test
	} __attribute__((packed));

	struct header_t0
	{
		common_header common;
		uint32_t bar0;
		uint32_t bar1;
		uint32_t bar2;
		uint32_t bar3;
		uint32_t bar4;
		uint32_t bar5;
		uint32_t cis_ptr;
		uint16_t sub_sys_vendor_id;
		uint16_t sub_sys_id;
		uint32_t exp_rom_addr;
		uint8_t capab_ptr;
		uint8_t reserved[7];
		uint8_t int_line;
		uint8_t int_pin;
		uint8_t min_grant;
		uint8_t max_latency;
	} __attribute__((packed));

	// PCI-to-PCI bridge
	struct header_t1
	{
		common_header common;
		uint32_t bar0;
		uint32_t bar1;
		uint8_t prim_bus_num;
		uint8_t sec_bus_num;
		uint8_t subor_bus_num;
		uint8_t sec_latency_timer;
		uint8_t io_base;
		uint8_t io_limit;
		uint16_t sec_status;
		uint16_t mem_base;
		uint16_t mem_limit;
		uint16_t prefetch_mem_base;
		uint16_t prefetch_mem_limit;
		uint32_t pref_base_upper32;
		uint32_t pref_limit_upper32;
		uint16_t io_base_upper16;
		uint16_t io_limit_upper16;
		uint8_t capab_ptr;
		uint8_t reserved[3];
		uint32_t exp_rom_addr;
		uint8_t int_line;
		uint8_t int_pin;
		uint16_t bridge_control;
	} __attribute__((packed));

	typedef void (*bus_scan_callback)(uint8_t bus, uint8_t device, uint8_t function, uint8_t header_type);

	constexpr uint32_t MAX_BUSSES = 256;
	constexpr uint32_t MAX_DEVICES = 32;
	constexpr uint16_t HEADER_TYPE_OFFSET = 0xe;
	constexpr uint16_t CLASS_CODE_OFFSET = 0xa;
	constexpr uint16_t PROGRAMMING_INTERFACE_OFFSET = 0x8;
	constexpr uint16_t BAR_0_LOW = 0x10;
	constexpr uint16_t BAR_0_HIGH = 0x12;

	enum class bar
	{
		BAR_0 = 0x10,
		BAR_1 = 0x14,
		BAR_2 = 0x18,
		BAR_3 = 0x1C,
		BAR_4 = 0x20,
		BAR_5 = 0x24
	};

	enum class class_codes
	{
		STORAGE = 0x1,
		NETWORK = 0x2,
		DISPLAY = 0x3,
		MULTI_MEDIA = 0x4,
		MEMORY = 0x5,
		BRIDGE = 0x6,
		COMMUNICATION = 0x7,
		BASE_SYS_PERIPHERALS = 0x8,
		INPUT = 0x9,
		DOCKING_STATION = 0xa,
		PROCESSOR = 0xb,
		SERIAL_BUS = 0xc,
		WIRELESS = 0xd,
		INTELLIGENT = 0xe,
		SATTELITE = 0xf,
		ENCRYPTION = 0x10,
		SIGNAL_PROCESSING = 0x11,
		PROCESSING_ACCELERATOR = 0x12,
		NON_ESSENTIAL = 0x13,
		CO_PROCESSOR = 0x40
	};

	uint16_t read_config_word(uint8_t bus, uint8_t device, uint8_t func, uint8_t offset)
	{
		uint32_t address;
		uint32_t lbus = (uint32_t)bus;
		uint32_t ldevice = (uint32_t)device;
		uint32_t lfunc = (uint32_t)func;
		uint16_t tmp = 0;

		/* create configuration address as per Figure 1 */
		address = (uint32_t)((lbus << 16) | (ldevice << 11) | (lfunc << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

		/* write out the address */
		io::outl(0xCF8, address);
		/* read in the data */
		/* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
		tmp = (uint16_t)((io::inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
		return (tmp);
	}

	uint16_t get_vendor(uint8_t bus, uint8_t device, uint8_t func)
	{
		return read_config_word(bus, device, func, 0);
	}

	uint32_t get_bar(uint8_t bus, uint8_t device, uint8_t func, bar bar)
	{
		uint16_t low = read_config_word(bus, device, func, static_cast<uint8_t>(bar));
		uint32_t high = static_cast<uint32_t>(read_config_word(bus, device, func, static_cast<uint8_t>(bar) + sizeof(uint16_t))) << 16;
		return high | low;
	}

	void scan_busses(size_t max_busses, bus_scan_callback scan_callback)
	{
		char buf1[16];
		char buf2[16];

		for (size_t bus = 0; bus < max_busses; bus++)
		{
			for (size_t device = 0; device < pci::MAX_DEVICES; device++)
			{
				uint8_t func = 0;

				if (get_vendor(bus, device, func) == 0xFFFF)
				{
					continue;
				}

				uint16_t header_type = read_config_word(bus, device, func, HEADER_TYPE_OFFSET);
				scan_callback(bus, device, func, header_type & ~0x80);
				if ((header_type & 0x80) != 0)
				{
					for (func = 1; func < 8; func++)
					{
						if (get_vendor(bus, device, func) != 0xFFFF)
						{
							uint16_t header_type = read_config_word(bus, device, func, HEADER_TYPE_OFFSET) & ~0x80;
							scan_callback(bus, device, func, header_type);
						}
					}
				}
			}
		}
	}

	void bus_scan_check(uint8_t bus, uint8_t device, uint8_t func, uint8_t header_type)
	{
		char buf1[16];
		char buf2[16];
		char buf3[16];
		char buf4[16];

		itoa(bus, buf1, 10);
		itoa(device, buf2, 10);
		itoa(func, buf3, 10);
		itoa(header_type, buf4, 10);

		vga::write_line(buf1, "\t\t", buf2, "\t\t", buf3, "\t\t", buf4);
	}
};

void pci::init()
{
	char buf1[16];
	char buf2[16];

	boot::boot_info *boot_info = boot::get_boot_info();

	uint16_t pci_mechanism = *(reinterpret_cast<uint16_t *>(boot_info->pci_support));

	if (pci_mechanism == 0 && !acpi::is_pci_available())
	{
		vga::write_line("pci not supported!");
	}
	else
	{
		uint16_t max_busses = *(reinterpret_cast<uint16_t *>(boot_info->last_pci_bus));

		if (max_busses == 0)
			max_busses = pci::MAX_BUSSES;

		itoa(pci_mechanism, buf1, 10);
		itoa(max_busses, buf2, 10);
		vga::write_line("pci mechanism: ", buf1);
		vga::write_line("pci max busses: ", buf2);

		vga::write_line("bus:\tdev:\tfunc:\theader:");
		pci::scan_busses(max_busses, bus_scan_check);
	}
}
