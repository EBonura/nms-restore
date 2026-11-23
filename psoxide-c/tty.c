/* TEST RUNTIME ONLY: this stores to the debug console unconditionally, and on a real console the
   store bus-errors. psoxide-c/build.sh defines PSOXIDE_TEST_RUNTIME; a game or disc build that
   pulls this file in stops here. */
#ifndef PSOXIDE_TEST_RUNTIME
#error "psoxide-c/tty.c is the test runtime's: it stores to the debug port and must not be linked into a shipping image"
#endif
/* Debug console output (expansion 2 port 0x1F802080), as read by the
   PSoXide emulator's --stop-on-tty and guest debug log. */
#include "tty.h"

static void tty_putc(char c) { *(volatile unsigned char *)0xBF802080 = (unsigned char)c; }

void tty_puts(const char *s) { while (*s) tty_putc(*s++); }

static void put_unsigned(unsigned long v, unsigned base, int width, char pad) {
    char buf[12]; int n = 0;
    do { unsigned d = v % base; buf[n++] = (char)(d < 10 ? '0' + d : 'a' + d - 10); v /= base; } while (v);
    while (n < width) buf[n++] = pad;
    while (n) tty_putc(buf[--n]);
}

void tty_printf(const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') { tty_putc(*fmt); continue; }
        fmt++;
        char pad = ' '; int width = 0;
        if (*fmt == '0') { pad = '0'; fmt++; }
        while (*fmt >= '0' && *fmt <= '9') width = width * 10 + (*fmt++ - '0');
        if (*fmt == 'l') fmt++;
        switch (*fmt) {
        case 'd': { long v = va_arg(ap, long); if (v < 0) { tty_putc('-'); v = -v; } put_unsigned((unsigned long)v, 10, width, pad); break; }
        case 'u': put_unsigned(va_arg(ap, unsigned long), 10, width, pad); break;
        case 'x': put_unsigned(va_arg(ap, unsigned long), 16, width, pad); break;
        case 's': tty_puts(va_arg(ap, const char *)); break;
        case 'c': tty_putc((char)va_arg(ap, int)); break;
        case '%': tty_putc('%'); break;
        default: tty_putc('?'); break;
        }
    }
    va_end(ap);
}
