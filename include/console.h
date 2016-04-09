#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <stddef.h>

int console_init(void);
void console_clear(void);
int console_puts(const char *);
int console_puts_err(const char *);
void console_put_ibuf(char c);
int console_get_line(char *buf, size_t *buf_len);

#endif // _CONSOLE_H_
