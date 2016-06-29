#ifndef WEENSYOS_LIB_H
#define	WEENSYOS_LIB_H
#include "types.h"

/*****************************************************************************
 * memcpy, memmove, memset, strlen
 *
 *   Our versions of important C library functions. */

void *memcpy(void *dst, const void *src, size_t n);
void *memmove(void *dst, const void *src, size_t n);
void *memset(void *x, int c, size_t n);
size_t strlen(const char *s);
size_t strnlen(const char *s, size_t maxlen);

/*****************************************************************************
 * va_list, va_start, va_arg, va_end
 *
 *   Our versions of C varargs macros (borrowed from NetBSD). */

typedef char *va_list;
#define	__va_size(type) \
	(((sizeof(type) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))
#define	va_start(ap, last) \
	((ap) = (va_list)&(last) + __va_size(last))
#define	va_arg(ap, type) \
	(*(type *)((ap) += __va_size(type), (ap) - __va_size(type)))
#define	va_end(ap)	((void)0)

/*****************************************************************************
 * console_printf(cursor, color, format, ...)
 *
 *   A version of printf() that prints messages to the x86 console.
 *
 *   The 'format' argument supports some of the C printf function's escapes:
 *   %d (to print an integer in decimal notation), %u (to print an unsigned
 *   integer in decimal notation), %x (to print an unsigned integer in
 *   hexadecimal notation), %c (to print a character), and %s (to print a
 *   string).  It also takes field widths and so forth, as in '%10s'.
 *
 *   The 'cursor' argument points to the initial position in CGA console
 *   memory.  CONSOLE_BEGIN corresponds to the upper-left corner.  It must be
 *   between CONSOLE_BEGIN and CONSOLE_END.
 *
 *   The 'color' argument equals the initial color used to print.  0x0700 is a
 *   good choice (grey on black).  The 'format' escape %C changes the color
 *   being printed.  It takes an integer from the parameter list.
 *
 *   The function returns the final position of the cursor. */

#define CONSOLE_BEGIN   ((uint16_t *) 0x000B8000)
#define CONSOLE_END     (CONSOLE_BEGIN + 80 * 25)

uint16_t *console_printf(uint16_t *cursor, int color,
			 const char *format, ...);

uint16_t *console_vprintf(uint16_t *cursor, int color,
			  const char *format, va_list val);

#endif /* !WEENSYOS_LIB_H */
