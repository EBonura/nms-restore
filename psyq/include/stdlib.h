/* PSoXide Psy-Q compatibility layer: stdlib pieces the games use
   (psyq/src/libc.rs). rand() is the classic 15-bit generator. */
#ifndef PSYQ_STDLIB_H
#define PSYQ_STDLIB_H
#define RAND_MAX 0x7FFF
int    abs(int x);
long   labs(long x);
int    rand(void);
void   srand(unsigned int seed);
int    atoi(const char *s);
long   atol(const char *s);
double atof(const char *s);
long   strtol(const char *s, char **end, int base);
double strtod(const char *s, char **end);
char  *itoa(int value, char *buf, int radix);
/* a heap from the end of the program to the stack reserve; free does nothing */
void  *malloc(unsigned int size);
void   free(void *p);
#endif
