#ifndef PSOXIDE_TTY_H
#define PSOXIDE_TTY_H
#include <stdarg.h>
void tty_puts(const char *s);
/* %d %u %x %s %c with optional zero pad and width; l is accepted and ignored. */
void tty_printf(const char *fmt, ...);
#endif
