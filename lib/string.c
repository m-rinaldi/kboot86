#include <string.h>
#include <stdint.h>

size_t strlen(const char *s)
{
    size_t count;

    if (!s)
        return 0;

    for (count = 0; s[count]; count++)
        ;

    return count;
}

char *strncpy(char *dest, const char *src, size_t n)
{
    size_t i;

    for (i = 0; n; i++, n--) {
        dest[i] = src[i];
        if ('\0' == src[i])
            break;
    }

    return dest;  
}

int strcmp(const char *s1, const char *s2)
{
    for (; *s1; s1++, s2++) {
        if (!*s2)
            return 1;

        if (*s1 < *s2)
            return -1;

        if (*s1 > *s2)
            return 1;
    }

    // *s1 is '\0'
    return !*s2 ? 0 : -1;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    for (; n && --n && *s1; s1++, s2++) {
        if (!*s2)
            return 1;

        if (*s1 < *s2)
            return -1;

        if (*s1 > *s2)
            return 1;
    }

    // all n characters were compared
    if (!n)
        return 0;

    // *s1 is '\0'
    return !*s2 ? 0 : -1;
}

// TODO use MOVSB
void *memcpy(void *dest, const void *src, size_t n)
{
    volatile uint8_t *d;
    uint8_t *s;

    for (d = (uint8_t *) dest, s = (uint8_t *) src; n; n--)
        *d++ = *s++;

    return dest;
}

void *memset(void *s, uint8_t c, size_t n)
{
    if (!s || !n)
        return s;

    while (n--)
        *((uint8_t *) s + n) = c;

    return s; 
}

void bzero(void *s, size_t n)
{
    memset(s, 0, n);
}
