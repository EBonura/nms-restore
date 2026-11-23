/* PSoXide Psy-Q compatibility layer: formatted output (psyq/src/libc.rs).
   printf writes to the debug console (expansion 2, 0x1F802080). Formats:
   %d %i %u %x %X %o %c %s %p %%, flags - 0 + space #, width, precision,
   * and the h/l length modifiers. %f %e %g print "?" and say so once on the
   debug console: QD Up never formats a double. */
#ifndef PSYQ_STDIO_H
#define PSYQ_STDIO_H
#include <stdarg.h>
int printf(const char *fmt, ...);
int sprintf(char *buf, const char *fmt, ...);
int puts(const char *s);
int putchar(int c);
#endif
