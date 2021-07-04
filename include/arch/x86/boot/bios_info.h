#pragma once

#include <lib/stdint.h>

namespace boot
{
    struct bios_info
    {
        uint16_t mem_list_size;
        uint16_t mem_list_addr;
    } __attribute__((packed));
}