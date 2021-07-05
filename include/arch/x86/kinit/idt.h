#pragma once

#include <lib/stdint.h>

namespace idt
{
    void init();
    void set_gate(size_t index, uint32_t base, uint16_t segment_selector, uint8_t flags);
};