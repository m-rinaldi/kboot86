#include <gdt.h>

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef struct {
    volatile unsigned int   accessed    :   1;
    unsigned int            read_write  :   1;
    unsigned int            dc          :   1;
    unsigned int            executable  :   1;
    unsigned int            one         :   1;
    unsigned int            privilege   :   2;
    unsigned int            present     :   1;
} __attribute__((packed)) access_t;

typedef struct {
    uint16_t        limit_0_15;
    uint16_t        base_0_15;
    uint8_t         base_16_23;
    access_t         access;
    unsigned int    limit_16_19 :   4;
    
    // flags
    unsigned int    zero        :   2;
    unsigned int    size        :   1;
    unsigned int    granularity :   1;

    uint8_t         base_24_31;
} __attribute__((packed)) gdt_entry_t;

typedef struct {
    uint16_t    size;
    uint32_t    offset;
} __attribute__((packed)) gdtr_t;

#define NUM_ENTRIES_MAX 3
// the GDT
static gdt_entry_t _[NUM_ENTRIES_MAX] __attribute__((aligned(4)));


/*******************************************************************************
 GDTR manipulation
*******************************************************************************/

// set offset of in-memory copy of the GDTR
static inline
void _mgdtr_set_offset(gdtr_t *gdtr, const gdt_entry_t *offset)
{
    // linear address of the table itself => paging applies!
    gdtr->offset = (uint32_t) offset;   
}

// set length of in-memory copy of the GDTR
static inline
int _mgdtr_set_length(gdtr_t *gdtr, uint16_t length)
{
    if (!length)
        return 1;

    gdtr->size = length * sizeof(gdt_entry_t) - 1;
    return 0;
}

// load the in-memory copy of the GDTR to the actual register
static void _gdtr_load(const gdtr_t *gdtr)
{
    asm volatile (
                    "lgdt (%0)\n"
                    : // no output
                    : "a" (gdtr)
                 );
}

static
int _gdtr_set(const gdt_entry_t *offset, uint16_t length)
{
    gdtr_t gdtr;

    _mgdtr_set_offset(&gdtr, offset);

    if (_mgdtr_set_length(&gdtr, length))
        return 1;

    _gdtr_load(&gdtr);

    return 0;
}
/******************************************************************************/


/*******************************************************************************
 manipulation of GDT entries
*******************************************************************************/
static inline
uint32_t _gdt_entry_get_base(const gdt_entry_t *gdte)
{
    uint32_t base;

    base  = gdte->base_0_15;
    base |= (uint32_t) gdte->base_16_23 << 16;
    base |= (uint32_t) gdte->base_24_31 << 24;

    return base;
}

static inline
uint32_t _gdt_entry_get_limit(const gdt_entry_t *gdte)
{
    uint32_t limit;

    limit  = gdte->limit_0_15;
    limit |= (uint32_t) gdte->limit_16_19 << 16;

    return limit; 
}

static inline
void _gdt_entry_set_base(gdt_entry_t *gdte, uint32_t base)
{
    gdte->base_0_15  = 0xffff & base;
    gdte->base_16_23 = 0x00ff & base >> 16;
    gdte->base_24_31 = 0x00ff & base >> 24;
}

static inline
void _gdt_entry_set_limit(gdt_entry_t *gdte, uint32_t limit)
{
    gdte->limit_0_15  = 0xffff & limit;
    gdte->limit_16_19 = 0x000f & limit >> 16;
}

// TODO
static inline
void _gdt_entry_set_flags(gdt_entry_t *gdte)
{
    gdte->zero = 0;
    gdte->size = 1;     // 32-bit protexted mode
    gdte->granularity = 1;  // limit is in 4kB-blocks
}

static inline
void _gdt_entry_set_access(gdt_entry_t *gdte, uint8_t privl, bool executable)
{
    bzero(&gdte->access, sizeof(gdte->access));

    // the access bit is set by the hardware

    gdte->access.read_write = 1;   // always read & write
    gdte->access.one = 1;
    gdte->access.executable = executable;
    gdte->access.privilege = 0x3 & privl;
    gdte->access.present = 1;
}

// set a GDT entry prior to adding it to the GDT table
static
void _gdt_entry_set(gdt_entry_t *gdte,
                    uint32_t base, uint32_t limit, bool executable, int privl)
{
    bzero(gdte, sizeof(*gdte));

    _gdt_entry_set_base(gdte, base);
    _gdt_entry_set_limit(gdte, limit);
    _gdt_entry_set_access(gdte, privl, executable); 
    _gdt_entry_set_flags(gdte);
}

static void _gdt_entry_set_code(gdt_entry_t *gdte, int privl)
{
    _gdt_entry_set(gdte, 0x00000000, 0x000fffff, true, privl);
}

static void _gdt_entry_set_data(gdt_entry_t *gdte, int privl)
{
    _gdt_entry_set(gdte, 0x00000000, 0x000fffff, false, privl);
}
/******************************************************************************/


static int _add_entry(uint16_t idx, const gdt_entry_t *gdte)
{
    if (idx >= NUM_ENTRIES_MAX)
        return 1;

    _[idx] = *gdte;
    return 0;
}


// call this function after modifying the GDT to update the segment selectors
static void _update_segments(unsigned int code, unsigned int data)
{
    // code and data are really entry indexes in the GDT 
    code *= 8;
    data *= 8;

    asm volatile (
                    "push %0\n\t"     // load CS
                    "push $1f\n\t"
                    "lret\n"
                    "1:\n\t"            // load DS, ES, FS, GS and SS
                    "movw %1, %%ax\n\t"
                    "movw %%ax, %%ds\n\t"
                    "movw %%ax, %%es\n\t"
                    "movw %%ax, %%fs\n\t"
                    "movw %%ax, %%gs\n\t"
                    "movw %%ax, %%ss\n\t"
                    :
                    : "g" (code), "g" (data)
                    : "%ax"  
                 );
}


int gdt_init(void)
{
    gdt_entry_t gdte;

    if (_gdtr_set(&_[0], NUM_ENTRIES_MAX))
        return 1;

    // 1st the null descriptor
    bzero(&gdte, sizeof(gdte));
    _add_entry(0, &gdte);

    // 2nd the code segment descriptor
    _gdt_entry_set_code(&gdte, 0);
    _add_entry(1, &gdte);

    // 3rd the data segment descriptor 
    _gdt_entry_set_data(&gdte, 0);
    _add_entry(2, &gdte);

    // update the segment selector registers
    _update_segments(1, 2);

    return 0;
}

#if 0
void gdtr_print(void)
{
    //kprintf("GDT; Size: %x; Base: %x\n", gdt_size, gdt);
}

void gdt_entry_print(struct gdt_entry *gdtep)
{
    kprintf("Base:          %x\n", gdtep->base_high << 24 
                | gdtep->base_mid << 16 | gdtep->base_low); 
    kprintf("Limit:         %x\n", (gdtep->gr_limit_high & 0x0f) << 16 
                        | gdtep->limit_low);
    kprintf("Access:        %x\n", gdtep->access);
    kprintf("Flags:         %x\n", gdtep->gr_limit_high);
}

// TODO rename to gdt_display()
void gdt_print(void)
{
    int i;
    for (i = 0; i < 3; i++) {
        kprintf("--- GDT Entry %0.1x ---\n", i);    
        gdt_entry_print(&gdt[i]);
    }
}
#endif
