#include <intr.h>
#include <idt.h>

#define IRQ_OFFSET  0x20
int intr_register_irq(uint8_t irq_num, void (*isr)(void))
{
    int idx = IRQ_OFFSET + irq_num;
    return idt_set_intr_gate(idx, isr); 
}

