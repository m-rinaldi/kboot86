#pragma once

#include <stdarg.h>

int kvsprintf(char *buf, const char *fmt, va_list args);
int kprintf(const char *fmt, ...);
