#pragma once

#include <stddef.h>
#include <stdint.h>

// use GCC's built in functions if possible
#define strlen(s)           __builtin_strlen    ((s))
#define strcmp(s1, s2)      __builtin_strcmp    ((s1), (s2))


#ifndef strlen
size_t strlen(const char *);
#endif

#ifndef strncpy
char *strncpy(char *dest, const char *src, size_t n);
#endif

#ifndef strcmp
int strcmp(const char *s1, const char *s2);
#endif

#ifndef strncmp
int strncmp(const char *s1, const char *s2, size_t n);
#endif

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, uint8_t c, size_t n);
void bzero(void *s, size_t n);
