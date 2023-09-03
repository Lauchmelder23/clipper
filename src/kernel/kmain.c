#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#	error "Please use a platform agnostic compiler"
#endif

#if !defined(__i386__)
#	error "Please use an ix86-elf compiler"
#endif

void kmain(void) {
	return;
}
