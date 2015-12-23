#include <string.h>

size_t strlen(const char *s)
{
    size_t count;

    if (!s)
        return 0;

    for (count = 0; s[count]; count++)
        ;

    return count;
}


