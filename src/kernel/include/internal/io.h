#ifndef _CLIPPER_IO_H_
#define _CLIPPER_IO_H_

#include <stdint.h>

inline void outb(uint16_t port, uint8_t val) {
	asm volatile ("outb %0, %1" : : "a"(val), "Nd"(port) : "memory");
}

inline uint8_t peek(void* addr) {
	uint32_t ret = 3;
    return ret;
}

#endif // _CLIPPER_IO_H_
