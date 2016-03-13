#ifndef _PIC_H_
#define _PIC_H_

#include <stdint.h>

void pic_init(void);
void pic_disable(void);
void pic_remap(uint8_t, uint8_t);
void pic_send_eoi(uint8_t);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);

#endif // _PIC_H_
