#pragma once

#include <stdarg.h>

int kvsprintf(char *buf, const char *format, va_list args);
int ksprintf(char *str, const char *format, ...);
int kprintf(const char *format, ...);
