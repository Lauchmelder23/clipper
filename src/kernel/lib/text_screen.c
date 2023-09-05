#include "internal/text_screen.h"
#include "internal/io.h"

struct text_screen_char {
	char character;
	uint8_t attribute;
} __attribute__((packed));

static volatile struct text_screen_char* videomem = (struct text_screen_char*)0xB8000;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

void tsinit(void) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void tsputch(char ch, uint8_t color) {
	const char tmp[2] = { ch, 0 };
	tsputs(tmp, color);
}

static void handle_special_char(char c) {
	switch (c) {
		case '\n': {
			cursor_x = 0;
			cursor_y++;

			if (cursor_y >= SCREEN_HEIGHT) {
				cursor_y = 0;
			}
		} break;

		case '\r': cursor_x = 0; break;
		case '\t': cursor_x += 4; break;
	}
}

void tsputs(const char* ch, uint8_t color) {
	struct text_screen_char tsch = {
		.character = '\0',
		.attribute = (color & 0x7F)
	};

	for(const char* c = ch; *c != '\0'; c++) {
		if ((uint8_t)(*c) < 0x20) {
			handle_special_char(*c);
			continue;
		}

		tsch.character = *c;

		videomem[cursor_y * SCREEN_WIDTH + cursor_x] = tsch;

		cursor_x++;
		if (cursor_x >= SCREEN_WIDTH) {
			cursor_y++;
			cursor_x = 0;

			if (cursor_y >= SCREEN_HEIGHT) {
				cursor_y = 0;	
			}
		}
	}
}
