#include "tty.h"
int main(void) { tty_printf("hello from C, %d %x %05d\n", -42, 0xbeef, 7); tty_puts("DONE\n"); return 0; }
