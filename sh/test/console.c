#include <console.h>

#include <stdio.h>
#include <string.h>

static void _remove_new_line(char *s)
{
    size_t len = strlen(s);

    if (len >= 1 && '\n' == s[len-1])
        s[len-1] = '\0';
}

int console_get_line(char *buf, size_t *buf_len)
{
    fgets(buf, *buf_len, stdin);
    _remove_new_line(buf);
    *buf_len = strlen(buf);

    return 0;
}
