#include <kstdio.h>

#include <stdio.h>

int kprintf(const char *format, ...)
{
	va_list args;
	int i;

	va_start(args, format);
	i = vprintf(format, args);
	va_end(args);

	return i;
}
