#include "lib.h"

/*****************************************************************************
 * lib.c
 *
 *   Useful C-library-like functions.  You don't need to understand their
 *   implementations.
 *
 *****************************************************************************/

/*****************************************************************************
 * memcpy, memmove, memset, and strlen
 *
 *   We must provide our own implementations of these basic functions. */

void *
memcpy(void *dst, const void *src, size_t n)
{
	const char *s = (const char *) src;
	char *d = (char *) dst;
	while (n-- > 0)
		*d++ = *s++;
	return dst;
}

void *
memmove(void *dst, const void *src, size_t n)
{
	const char *s = (const char *) src;
	char *d = (char *) dst;
	if (s < d && s + n > d) {
		s += n, d += n;
		while (n-- > 0)
			*--d = *--s;
	} else
		while (n-- > 0)
			*d++ = *s++;
	return dst;
}

void *
memset(void *v, int c, size_t n)
{
	char *p = (char *) v;
	while (n-- > 0)
		*p++ = c;
	return v;
}

size_t
strlen(const char *s)
{
	size_t n;
	for (n = 0; *s != '\0'; ++s)
		++n;
	return n;
}

size_t
strnlen(const char *s, size_t maxlen)
{
	size_t n;
	for (n = 0; n != maxlen && *s != '\0'; ++s)
		++n;
	return n;
}


/*****************************************************************************
 * console_vprintf
 *
 *   Print a message onto the console, starting at the given cursor position. */

static uint16_t *
console_putc(uint16_t *cursor, unsigned char c, int color)
{
	if (cursor >= CONSOLE_END)
		cursor = CONSOLE_BEGIN;
	if (c == '\n') {
		int pos = (cursor - CONSOLE_BEGIN) % 80;
		for (; pos != 80; pos++)
			*cursor++ = ' ' | color;
	} else
		*cursor++ = c | color;
	return cursor;
}

static const char upper_digits[] = "0123456789ABCDEF";
static const char lower_digits[] = "0123456789abcdef";

static char *
fill_numbuf(char *numbuf_end, uint32_t val, int base, const char *digits,
	    int precision)
{
	*--numbuf_end = '\0';
	if (precision != 0 || val != 0)
		do {
			*--numbuf_end = digits[val % base];
			val /= base;
		} while (val != 0);
	return numbuf_end;
}

#define FLAG_ALT		(1<<0)
#define FLAG_ZERO		(1<<1)
#define FLAG_LEFTJUSTIFY	(1<<2)
#define FLAG_SPACEPOSITIVE	(1<<3)
#define FLAG_PLUSPOSITIVE	(1<<4)
static const char flag_chars[] = "#0- +";

uint16_t *
console_vprintf(uint16_t *cursor, int color, const char *format, va_list val)
{
	int flags, width, zeros, precision, negative, numeric, len;
#define NUMBUFSIZ 20
	char numbuf[NUMBUFSIZ];
	char *data;

	for (; *format; ++format) {
		if (*format != '%') {
			cursor = console_putc(cursor, *format, color);
			continue;
		}

		// process flags
		flags = 0;
		for (++format; *format; ++format) {
			const char *flagc = flag_chars;
			while (*flagc != '\0' && *flagc != *format)
				++flagc;
			if (*flagc == '\0')
				break;
			flags |= (1 << (flagc - flag_chars));
		}

		// process width
		width = -1;
		if (*format >= '1' && *format <= '9') {
			for (width = 0; *format >= '0' && *format <= '9'; )
				width = 10 * width + *format++ - '0';
		} else if (*format == '*') {
			width = va_arg(val, int);
			++format;
		}

		// process precision
		precision = -1;
		if (*format == '.') {
			++format;
			if (*format >= '0' && *format <= '9') {
				for (precision = 0; *format >= '0' && *format <= '9'; )
					precision = 10 * precision + *format++ - '0';
			} else if (*format == '*') {
				precision = va_arg(val, int);
				++format;
			}
			if (precision < 0)
				precision = 0;
		}

		// process main conversion character
		negative = 0;
		numeric = 0;
		switch (*format) {
		case 'd': {
			int x = va_arg(val, int);
			data = fill_numbuf(numbuf + NUMBUFSIZ, x > 0 ? x : -x, 10, upper_digits, precision);
			if (x < 0)
				negative = 1;
			numeric = 1;
			break;
		}
		case 'u': {
			unsigned x = va_arg(val, unsigned);
			data = fill_numbuf(numbuf + NUMBUFSIZ, x, 10, upper_digits, precision);
			numeric = 1;
			break;
		}
		case 'x': {
			unsigned x = va_arg(val, unsigned);
			data = fill_numbuf(numbuf + NUMBUFSIZ, x, 16, lower_digits, precision);
			numeric = 1;
			break;
		}
		case 'X': {
			unsigned x = va_arg(val, unsigned);
			data = fill_numbuf(numbuf + NUMBUFSIZ, x, 16, upper_digits, precision);
			numeric = 1;
			break;
		}
		case 's':
			data = va_arg(val, char *);
			break;
		case 'C':
			color = va_arg(val, int);
			goto done;
		case 'c':
			data = numbuf;
			numbuf[0] = va_arg(val, int);
			numbuf[1] = '\0';
			break;
		normal:
		default:
			data = numbuf;
			numbuf[0] = (*format ? *format : '%');
			numbuf[1] = '\0';
			if (!*format)
				format--;
			break;
		}

		if (precision >= 0)
			len = strnlen(data, precision);
		else
			len = strlen(data);
		if (numeric && negative)
			negative = '-';
		else if (flags & FLAG_PLUSPOSITIVE)
			negative = '+';
		else if (flags & FLAG_SPACEPOSITIVE)
			negative = ' ';
		else
			negative = 0;
		if (numeric && precision > len)
			zeros = precision - len;
		else if ((flags & (FLAG_ZERO | FLAG_LEFTJUSTIFY)) == FLAG_ZERO
			 && numeric && precision < 0
			 && len + !!negative < width)
			zeros = width - len - !!negative;
		else
			zeros = 0;
		width -= len + zeros + !!negative;
		for (; !(flags & FLAG_LEFTJUSTIFY) && width > 0; --width)
			cursor = console_putc(cursor, ' ', color);
		if (negative)
			cursor = console_putc(cursor, negative, color);
		for (; zeros > 0; --zeros)
			cursor = console_putc(cursor, '0', color);
		for (; len > 0; ++data, --len)
			cursor = console_putc(cursor, *data, color);
		for (; width > 0; --width)
			cursor = console_putc(cursor, ' ', color);
	done: ;
	}

	return cursor;
}

uint16_t *
console_printf(uint16_t *cursor, int color, const char *format, ...)
{
	va_list val;
	va_start(val, format);
	cursor = console_vprintf(cursor, color, format, val);
	va_end(val);
	return cursor;
}
