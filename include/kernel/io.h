#pragma once

#include <stdint.h>

class io
{
public:
	static inline uint8_t inb(uint16_t port);
	static inline void outb(uint16_t port, uint8_t byte);
	static inline void wait();
};
