#include <io.h>
#include <kint.h>

/*******************************************************************************
 inb: read a byte from port number port
*******************************************************************************/
kuint8_t inb(kuint16_t port)
{
	kuint8_t value;
	asm volatile("inb %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

kuint16_t inw(kuint16_t port)
{
	kuint16_t value;
	asm volatile("inw %1, %0" : "=a" (value) : "dN" (port));
	return value;
}

/*******************************************************************************
 outb: write value to port number port
*******************************************************************************/
void outb(kuint16_t port, kuint8_t value)
{
	asm volatile("outb %1, %0" : : "dN" (port),  "a" (value));
	return;
}
