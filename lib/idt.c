/*
 * IDT: Interrupt Description Table
 */
#include <idt.h>

#include <stddef.h>
#include <stdint.h>

#include <intr.h>
#include <eflags.h>
#include <faults.h>

#define IDT_CS              0x0008
#define IDT_NUM_GATES_MAX   0x100


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


typedef struct {
    idt_gate_t  gate[256];
} __attribute__((packed)) idt_t;

// mapping of the IDTR processor register
typedef struct {
    uint16_t    limit;      // length of the IDT in bytes - 1
    uint32_t    base;       // linear address where the IDT starts (INT 0)
} __attribute__((packed)) idtr_t;

static volatile idt_t _ __attribute__((aligned(8)));

#define GATE_TRAP   0
#define GATE_INTR   1
#define GATE_TASK   2
static
int _set_gate(unsigned int index,
              void (*isr)(void),
              uint16_t selector, unsigned int privi_level, int type)
{
    idt_gate_t gate;

    if (index >= IDT_NUM_GATES_MAX)
        return 1;

    if (privi_level >= 4)
        return 1;

    // TODO move this inside the switch, intrs & traps have a lot in common
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
    _.gate[index] = gate;    

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


static inline
idtr_t _get_idtr(void)
{
    idtr_t idtr;
    asm volatile
        (
            "# _get_idtr()\n\t"
            "sidt %0\n\t"
            :   // no output operands
            :   "m" (idtr)
            :   "memory"
        );
    return idtr;
}

static inline
void _set_idtr(idtr_t idtr)
{
    asm volatile
        (
            "# _set_idtr()\n\t"
            "lidt %0\n\t"
            :   // no output operands
            : "m" (idtr)
            : "memory"
        ); 
}

int idt_init(void)
{
    idtr_t  idtr;
    bool intr_flag;

    // save intr_flag from FLAGS
    intr_flag = eflags_get_intr_flag();
    // disable intr_flag in FLAGS
    intr_disable();

    idtr.base = (uint32_t) &_;
    idtr.limit = IDT_NUM_GATES_MAX * sizeof(idt_gate_t) - 1;

    // set up the whole IDT with default entries
    {
        int i;
        for (i = 0; i < IDT_NUM_GATES_MAX; i++)
            idt_set_intr_gate(i, (void (*)(void)) NULL);
    }

    // set up common handler for hardware exceptions
    {
        int i;
        for (i = 0; i < 32; i++)
            idt_set_intr_gate(i, exception_handler);
    }

    // XXX
    {
        extern void dummy_isr(void);

        extern void isr32(void);
        extern void isr33(void);
        extern void isr34(void);
        extern void isr35(void);
        extern void isr36(void);
        extern void isr37(void);
        extern void isr38(void);
        extern void isr39(void);
        extern void isr40(void);
        extern void isr41(void);
        extern void isr42(void);
        extern void isr43(void);
        extern void isr44(void);
        extern void isr45(void);
        extern void isr46(void);
        extern void isr47(void);

        idt_set_intr_gate(32, isr32);
        idt_set_intr_gate(33, isr33);
        idt_set_intr_gate(34, isr34);
        idt_set_intr_gate(35, isr35);
        idt_set_intr_gate(36, isr36);
        idt_set_intr_gate(37, isr37);
        idt_set_intr_gate(38, isr38);
        idt_set_intr_gate(39, isr39);
        idt_set_intr_gate(40, isr40);
        idt_set_intr_gate(41, isr41);
        idt_set_intr_gate(41, isr42);
        idt_set_intr_gate(43, isr43);
        idt_set_intr_gate(44, isr44);
        idt_set_intr_gate(45, isr45);
        idt_set_intr_gate(46, isr46);
        idt_set_intr_gate(47, isr47);
    }

    _set_idtr(idtr);

    // restore saved intr_flag
    eflags_set_intr_flag(intr_flag);

    return 0;
}

void idt_display(void)
{
    idtr_t idtr;

    idtr = _get_idtr();

    // TODO
    (void) idtr;
}
