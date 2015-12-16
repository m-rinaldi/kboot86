#include <pic.h>
#include <x86_io.h>
#include <stdint.h>

#define MASTER      0x20
#define SLAVE       0xa0

#define MASTER_CMD  MASTER
#define MASTER_DATA (MASTER+1)
#define SLAVE_CMD   SLAVE
#define SLAVE_DATA  (SLAVE+1)


#define CMD_8086    0x01
#define CMD_INIT    0x11
#define CMD_EOI     0x20

/*
    mask for IRQs
    IRQ lines are individually enabled/disabled
        0   enabled
        1   disabled
 */

static inline
void _master_send_cmd(uint8_t cmd)
{
    x86_outb(MASTER_CMD, cmd);
}

static inline
void _slave_send_cmd(uint8_t cmd)
{
    x86_outb(SLAVE_CMD, cmd);
}

static inline
uint8_t _master_get_data(void)
{
    return x86_inb(MASTER_DATA); 
}

static inline
uint8_t _slave_get_data(void)
{
    return x86_inb(SLAVE_DATA);
}

static inline
void _master_set_data(uint8_t data)
{
    x86_outb(MASTER_DATA, data);
}

static inline
void _slave_set_data(uint8_t data)
{
    x86_outb(SLAVE_DATA, data);
}

void pic_remap(uint8_t master_off, uint8_t slave_off)
{
    uint8_t master_mask, slave_mask;

    // save masks
    master_mask = _master_get_data();
    slave_mask  = _slave_get_data(); 

    _master_send_cmd(CMD_INIT);
    _slave_send_cmd(CMD_INIT);
    
    _master_set_data(master_off);
    _slave_set_data(slave_off);

    // tell master PIC that there is a slave PIC at IRQ2
    _master_set_data(4);
    // tell slave PIC its cascade identity at IRQ9 
    _slave_set_data(2);

    _master_set_data(CMD_8086);
    _slave_set_data(CMD_8086);    

    // restore masks
    _master_set_data(master_mask);
    _slave_set_data(slave_mask);
}

void pic_enable(void)
{
    x86_outb(MASTER_DATA, 0);
    x86_outb(SLAVE_DATA, 0);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        _slave_send_cmd(CMD_EOI);

    _master_send_cmd(CMD_EOI);
}
