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

// TODO use MOVSB
void *memcpy(void *dest, const void *src, size_t n)
{
    volatile uint8_t *d;
    uint8_t *s;

    for (d = (uint8_t *) dest, s = (uint8_t *) src; n; n--)
        *d++ = *s++;

    return dest;
}
