#include <arch/x86/kinit/idt.h>
#include <arch/x86/kinit/isr.h>
#include <kernel/drivers/vga.h>
#include <lib/stdint.h>
#include <lib/string.h>

namespace idt
{
    struct idt_entry_t
    {
        uint16_t base_low;
        uint16_t segment_selector;
        uint8_t zero;
        uint8_t flags;
        uint16_t base_high;
    }__attribute__((packed));

    struct idt_ptr_t
    {
        uint16_t limit;
        uint32_t base;
    }__attribute__((packed));

    idt_entry_t *idt_base;
    idt_ptr_t idt_ptr;
};

extern "C" void idt_flush(uint32_t ptr);

void idt::init()
{
    idt_ptr.base = 0x0;
    idt_ptr.limit = sizeof(idt_entry_t) * 256 - 1;
    idt_base = reinterpret_cast<idt_entry_t*>(idt_ptr.base);
    
    isr::init();

    idt_flush(reinterpret_cast<uint32_t>(&idt_ptr));
}

void idt::set_gate(size_t index, uint32_t base, uint16_t segment_selector, uint8_t flags)
{
    char buf[16];
    utoa(index, buf, 10);
    char buf2[16];
    utoa(base, buf2, 16);

    vga::write_line("idt index: ", buf, " base high: ", buf2);

    idt_base[index].base_high = (base >> 16) & 0xFFFF;
    idt_base[index].base_low = static_cast<uint16_t>(base);

    idt_base[index].segment_selector = segment_selector;
    idt_base[index].zero = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_base[index].flags = flags /* | 0x60 */;
}