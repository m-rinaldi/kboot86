/*
 * IDT: Interrupt Description Table
 */

#include <stdint.h>
#include <sys/types.h>
#include <kernel/isr.h>

#define IDT_CS      0x0008
#define NUM_ISRS    0x20

// mapping of the IDTR processor register
typedef struct {
    uint16_t    limit;      // length of the IDT in bytes - 1
    uint32_t    base;       // linear address where the IDT starts (INT 0)
} __attribute__((packed)) idtr_t;

typedef struct {
    unsigned int    gate_type   : 4;

    // intrs & trap gates: segment selector
    // task gates   gates: TSS descriptor
    unsigned int    s           : 1; 

    // Descriptor Privilege Level 0..3 (0 ist the highest privilege)
    unsigned int    dpl         : 2; 
    
    // present: whether the segment is present in RAM
    unsigned int    p           : 1; 
} __attribute__((packed)) gate_attr_t;

// entries in the IDT are called "gates"
typedef struct {
    uint16_t        offset_lo;
    uint16_t        selector;
    uint8_t         zero;
    gate_attr_t     attr;
    uint16_t        offset_hi;
} __attribute__((packed)) idt_gate_t;


static struct {
    volatile idt_gate_t *idt;
    void (*handlers[NUM_ISRS])(isr_saved_regs_t);
    
} _;



#define GATE_TRAP   0
#define GATE_INTR   1
#define GATE_TASK   2
static
int _set_gate(unsigned int index,
              void (*isr)(void),
              uint16_t selector, unsigned int privi_level, int type)
{
    idt_gate_t gate;

    if (index >= NUM_ISRS)
        return 1;

    if (privi_level >= 4)
        return 1;

    // TODO move this inside the switch,i ntrs & traps have a lot in common
    gate.offset_lo  = 0xffff & (uint32_t) isr;
    gate.offset_hi  = (0xffff0000 & (uint32_t) isr) >> 16;
    gate.selector   = selector;
    gate.zero       = 0;

    switch (type) {
        case GATE_TRAP:
            gate.attr.gate_type = 0xf;
            break;
        case GATE_INTR:
            gate.attr.gate_type = 0xe;
            break;
        case GATE_TASK:
            gate.attr.gate_type = 0x5;
            break;
        default:
            return 1;
    }

    gate.attr.dpl   = privi_level;
    gate.attr.p     = 1;

    // TODO bit S ???
    gate.attr.s     = 0;

    // place the gate in the IDT
    _.idt[index] = gate;    

    return 0;
}

int idt_set_trap_gate(unsigned int index, void (*isr)(void))
{
    return _set_gate(index, isr, IDT_CS, 0, GATE_TRAP);
}

int idt_set_intr_gate(unsigned int index, void (*isr)(void))
{
    return _set_gate(index, isr, IDT_CS, 0, GATE_INTR);
}

int idt_set_task_gate(unsigned int index, void (*isr)(void))
{
    return _set_gate(index, isr, IDT_CS, 0, GATE_TASK); 
}



static inline idtr_t _get_idtr(void)
{
    idtr_t idtr;
    asm volatile ("sidt %0" : : "m" (idtr));
    return idtr;
}

static inline void _set_idtr(idtr_t idtr)
{
    asm volatile ("lidt %0" : : "m" (idtr)); 
}

int idt_init(uint32_t base)
{
    idtr_t  idtr;

    idtr.base = base;
    idtr.limit = NUM_ISRS - 1;

    {
        int i;
        for (i = 0; i < NUM_ISRS; i++)
            _.handlers[i] = NULL;
    }
    // TODO

    _set_idtr(idtr);

    return 0;
}

void idt_display(void)
{
    idtr_t idtr;

    idtr = _get_idtr();

    // TODO
    (void) idtr;
}
