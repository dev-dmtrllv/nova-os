#include <kernel/io.h>

inline uint8_t io::inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %0, %1"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

inline void io::outb(uint16_t port, uint8_t byte)
{
	asm volatile("outb {%1, %0 | %0, %1}"
				 :
				 : "a"(byte),
				   "Nd"(port));
}

inline void io::wait()
{
	io::outb(0x80, 0);
}
