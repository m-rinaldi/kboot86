#ifndef _IDT_H_
#define _IDT_H_

#include <stdint.h>

int idt_init(uint32_t base);
int idt_set_trap_gate(unsigned int index, void (*isr) (void));
int idt_set_intr_gate(unsigned int index, void (*isr) (void));
int idt_set_task_gate(unsigned int index, void (*isr) (void));
void idt_display(void);

#endif // _IDT_H_
