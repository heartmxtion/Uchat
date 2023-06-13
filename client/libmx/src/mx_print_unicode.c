#include "../inc/libmx.h"

void mx_print_unicode(wchar_t c) {
    char unicode[4];

	if (c < 0x80) {
		unicode[0] = (c >> 0 & 0x7F) | 0x00;
		write(1, &unicode, 1);
	} else if (c < 0x0800) {
		unicode[0] = (c >> 6 & 0x1F) | 0xC0;
		unicode[1] = (c >> 0 & 0x3F) | 0x80;
		write(1, &unicode, 2);
	} else if (c < 0x010000) {
		unicode[0] = (c >> 12 & 0x0F) | 0xE0;
		unicode[1] = (c >> 6 & 0x3F) | 0x80;
		unicode[2] = (c >> 0 & 0x3F) | 0x80;
		write(1, &unicode, 3);
	} else {
		unicode[0] = (c >> 18 & 0x07) | 0xF0;
		unicode[1] = (c >> 12 & 0x3F) | 0x80;
		unicode[2] = (c >> 6 & 0x3F) | 0x80;
		unicode[3] = (c >> 0 & 0x3F) | 0x80;
		write(1, &unicode, 4);
	}
}

