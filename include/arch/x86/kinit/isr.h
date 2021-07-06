#pragma once

#include <lib/stdint.h>

#define ISR0 0
#define ISR1 1
#define ISR2 2
#define ISR3 3
#define ISR4 4
#define ISR5 5
#define ISR6 6
#define ISR7 7
#define ISR8 8
#define ISR9 9
#define ISR10 10
#define ISR11 11
#define ISR12 12
#define ISR13 13
#define ISR14 14
#define ISR15 15
#define ISR16 16
#define ISR17 17
#define ISR18 18

#define IRQ0 32
#define IRQ1 33
#define IRQ2 34
#define IRQ3 35
#define IRQ4 36
#define IRQ5 37
#define IRQ6 38
#define IRQ7 39
#define IRQ8 40
#define IRQ9 41
#define IRQ10 42
#define IRQ11 43
#define IRQ12 44
#define IRQ13 45
#define IRQ14 46
#define IRQ15 47

namespace isr
{
    struct registers_t
    {
        uint32_t ds;                                     // Data segment selector
        uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax; // Pushed by pusha.
        uint32_t int_no, err_code;                       // Interrupt number and error code (if applicable)
        uint32_t eip, cs, eflags, useresp, ss;           // Pushed by the processor automatically.
    };

    typedef void (*interrupt_handler_callback)(isr::registers_t *);

    void init();
    void register_interrupt_handler(size_t, interrupt_handler_callback);
    void set_irq_mask(unsigned char irq_line);
    void clear_irq_mask(unsigned char irq_line);
};