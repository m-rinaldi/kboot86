#ifndef _MONITOR_H_
#define _MONITOR_H_

#include <stdint.h>

int monitor_init(void);
void monitor_clear(void);
void monitor_set_attr(uint8_t);
int monitor_puts(const char *);

#endif //_MONITOR_H_
