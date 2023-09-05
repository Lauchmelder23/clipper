#include "stdlib.h"

char* itoa(int value, char* buffer, int base) {
	char* ptr;
	char* value_start;

	if (base < 2 || base > 35) {
		*buffer = '\0';
		return buffer;
	}

	ptr = buffer;

	if (value < 0 && base == 10) {
		*ptr++ = '-';
	}

	value_start = ptr;

	do {
		short remainder = value % base;
		*ptr++ = "zyxwvutsrqponmlkjihgfedcba9876543210123456789abcdefghijklmnopqrstuvwxyz"[35 + remainder];

		value /= base;
	} while(value);

	*ptr-- = '\0';

	while (value_start < ptr) {
		char tmp = *ptr;
		*ptr-- = *value_start;
		*value_start++ = tmp;
	}

	return buffer;
}

char* utoa(unsigned int value, char* buffer, int base) {
	char* ptr;
	char* value_start;

	if (base < 2 || base > 35) {
		*buffer = '\0';
		return buffer;
	}

	ptr = value_start = buffer;

	do {
		short remainder = value % base;
		*ptr++ = "0123456789abcdefghijklmnopqrstuvwxyz"[remainder];

		value /= base;
	} while(value != 0);

	*ptr-- = '\0';

	while (value_start < ptr) {
		char tmp = *ptr;
		*ptr-- = *value_start;
		*value_start++ = tmp;
	}

	return buffer;
}

