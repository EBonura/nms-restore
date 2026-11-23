/* PSoXide Psy-Q compatibility layer: string and memory functions.
   memcpy, memset and memcmp are psx-rt's; the rest is psyq/src/libc.rs. */
#ifndef PSYQ_STRING_H
#define PSYQ_STRING_H
typedef unsigned int size_t;
void  *memcpy(void *d, const void *s, size_t n);
void  *memmove(void *d, const void *s, size_t n);
void  *memset(void *d, int c, size_t n);
int    memcmp(const void *a, const void *b, size_t n);
char  *strcpy(char *d, const char *s);
char  *strncpy(char *d, const char *s, size_t n);
char  *strcat(char *d, const char *s);
int    strcmp(const char *a, const char *b);
int    strncmp(const char *a, const char *b, size_t n);
size_t strlen(const char *s);
char  *strchr(const char *s, int c);
void   bzero(void *d, size_t n);
#endif
