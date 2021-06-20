#pragma once

#include <stdint.h>

class io
{
public:
	static inline uint16_t inb(uint16_t port)
	{
		uint8_t result;
    	__asm__ volatile("inb %1, %0" : "=a" (result) : "Nd" (port));   
    	return result;
	}

	static inline void outb(uint16_t port, uint8_t byte)
	{
		__asm__ volatile("outb %0, %1" :: "a" (byte), "Nd" (port));
	}

	static inline void wait()
	{
		io::outb(0x80, 0);
	}
};
