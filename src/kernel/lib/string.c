#include "string.h"

#include <stdint.h>

static volatile char* video = (volatile char*)0xB8000;
static uint8_t cursor_x = 0, cursor_y = 0;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

static inline void advance_cursor(void) {
	cursor_x++;
	if (cursor_x >= SCREEN_WIDTH) {
		cursor_x = 0;
		cursor_y++;

		if (cursor_y >= SCREEN_HEIGHT) {
			cursor_y = 0;
		}
	}
}

static inline void linefeed(void) {
	cursor_y++;
	if (cursor_y >= SCREEN_HEIGHT) {
		cursor_y = 0;
	}
}

void putch(char c) {
	switch(c) {
	case '\r':	cursor_x = 0; return;
	case '\n':	linefeed(); return;
	}

	video[cursor_y * (SCREEN_WIDTH * 2) + (cursor_x * 2)] = c;
	advance_cursor();
}

void puts(const char* str) {
	for (const char* c = str; *c != '\0'; c++) {
		putch(*c);
	}
}
