#include <intr.h>
#include <idt.h>

#define IRQ_OFFSET  0x20
// TODO rename to "intr_install_irq_handler"
int intr_register_irq(uint8_t irq_num, void (*isr)(void))
{
    int idx = IRQ_OFFSET + irq_num;
    return idt_set_intr_gate(idx, isr); 
}

