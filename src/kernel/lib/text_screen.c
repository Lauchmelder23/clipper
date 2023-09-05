#include "internal/text_screen.h"

#include <stdarg.h>
#include "internal/io.h"
#include "stdlib.h"

struct text_screen_char {
	char character;
	uint8_t attribute;
} __attribute__((packed));

static volatile struct text_screen_char* videomem = (struct text_screen_char*)0xB8000;
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

enum printf_format_specifier {
	FORMAT_SPECIFIER_INT,
	FORMAT_SPECIFIER_UNSIGNED,
	FORMAT_SPECIFIER_HEXADECIMAL,
	FORMAT_SPEFICIER_UNKNOWN
};

void tsinit(void) {
	outb(0x3D4, 0x0A);
	outb(0x3D5, 0x20);
}

void tsputch(char ch, uint8_t color) {
	const char tmp[2] = { ch, 0 };
	tsputs(tmp, color);
}

static 
void handle_special_char(char c) {
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

static 
const char* decode_format_spefifier(const char* str, enum printf_format_specifier* data) {
	switch (*str) {
		case 'd':
		case 'i':
			*data = FORMAT_SPECIFIER_INT;
			return str+1;

		case 'o':
		case 'u':
			*data = FORMAT_SPECIFIER_UNSIGNED;
			return str+1;

		case 'x':
		case 'X':
			*data = FORMAT_SPECIFIER_HEXADECIMAL;
			return str+1;
	}

	*data = FORMAT_SPEFICIER_UNKNOWN;
	return str;
}

static 
const char* handle_format_specifier(const char* str, va_list* args) {
	str++;
	char tmp[16];
	
	enum printf_format_specifier specifier;
	str = decode_format_spefifier(str, &specifier);
	switch (specifier) {
		case FORMAT_SPECIFIER_INT: {
			int value = va_arg(*args, int);
			tsputs(itoa(value, tmp, 10), TEXT_SCREEN_BG_BLACK | TEXT_SCREEN_FG_WHITE);
		} break;

		case FORMAT_SPECIFIER_UNSIGNED: {
			unsigned int value = va_arg(*args, unsigned int);
			tsputs(utoa(value, tmp, 10), TEXT_SCREEN_BG_BLACK | TEXT_SCREEN_FG_WHITE);
		} break;


		case FORMAT_SPECIFIER_HEXADECIMAL: {
			unsigned int value = va_arg(*args, unsigned int);
			tsputs(utoa(value, tmp, 16), TEXT_SCREEN_BG_BLACK | TEXT_SCREEN_FG_WHITE);
		} break;
		default: return str;
	}

	return str-1;
}

void tsprintf(const char* format, ...) {
	struct text_screen_char tsch = {
		.character = '\0',
		.attribute = TEXT_SCREEN_BG_BLACK | TEXT_SCREEN_FG_GRAY
	};

	va_list args;
	va_start(args, format);

	for (const char* c = format; *c != '\0'; c++) {
		if (*c == '%') {
			c = handle_format_specifier(c, &args);
			continue;
		}

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

	va_end(args);
}
