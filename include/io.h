#ifndef _IO_H_
#define _IO_H_

#include <kint.h>

kuint8_t inb(kuint16_t);
kuint16_t inw(kuint16_t);
void outb(kuint16_t, kuint8_t);

#endif /* _IO_H_ */
