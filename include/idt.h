#pragma once

#include <stdint.h>

int idt_init(void);
int idt_set_trap_gate(unsigned int index, void (*isr) (void));
int idt_set_intr_gate(unsigned int index, void (*isr) (void));
int idt_set_task_gate(unsigned int index, void (*isr) (void));
void idt_display(void);
