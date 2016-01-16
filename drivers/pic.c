#include <pic.h>
#include <io.h>
#include <stdint.h>
#include <console.h>

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
    This is done by means of the IMR (Interrupt Mask Register)
        0   enabled
        1   disabled
*/
static inline
void _master_send_cmd(uint8_t cmd)
{
    outb(MASTER_CMD, cmd);
}

static inline
void _slave_send_cmd(uint8_t cmd)
{
    outb(SLAVE_CMD, cmd);
}

static inline
uint8_t _master_get_data(void)
{
    return inb(MASTER_DATA); 
}

static inline
uint8_t _master_get_imr(void)
{
    return _master_get_data();
}

static inline
uint8_t _slave_get_data(void)
{
    return inb(SLAVE_DATA);
}

static inline
uint8_t _slave_get_imr(void)
{
    return _slave_get_data();
}

static inline
void _master_set_data(uint8_t data)
{
    outb(MASTER_DATA, data);
}

static inline
void _master_set_imr(uint8_t imr)
{
    _master_set_data(imr);
}

static inline
void _slave_set_data(uint8_t data)
{
    outb(SLAVE_DATA, data);
}

static inline
void _slave_set_imr(uint8_t imr)
{
    _slave_set_data(imr);   
}

void pic_remap(uint8_t master_off, uint8_t slave_off)
{
    uint8_t master_imr, slave_imr;

    // save masks
    master_imr = _master_get_imr();
    slave_imr  = _slave_get_imr(); 

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
    _master_set_imr(master_imr);
    _slave_set_imr(slave_imr);
}

void pic_disable(void)
{
    // disable all irq lines
    _master_set_imr(0xff);
    _slave_set_imr(0xff);
}

#define MASTER_OFFSET   32
#define SLAVE_OFFSET    40
void pic_init(void)
{
    pic_disable();
    pic_remap(MASTER_OFFSET, SLAVE_OFFSET);
}

void pic_enable_irq(uint8_t irq)
{
    uint8_t imr;
    uint8_t (*get_imr)(void);
    void (*set_imr)(uint8_t);

    if (irq < 8) {
        get_imr = _master_get_imr;
        set_imr = _master_set_imr;
    } else {
        get_imr = _slave_get_imr;
        set_imr = _slave_set_imr;
        irq -= 8;
    }

    imr = get_imr();
    imr &= ~((uint8_t)1 << irq);
    set_imr(imr);
}

void pic_disable_irq(uint8_t irq)
{
    uint8_t imr;
    uint8_t (*get_imr)(void);
    void (*set_imr)(uint8_t);

    if (irq < 8) {
        get_imr = _master_get_imr;
        set_imr = _master_set_imr;
    } else {
        get_imr = _slave_get_imr;
        set_imr = _slave_set_imr;
        irq -= 8;
    }

    imr = get_imr();
    imr |= ((uint8_t)1 << irq);
    set_imr(imr);
}

void pic_send_eoi(uint8_t irq)
{
    if (irq >= 8)
        _slave_send_cmd(CMD_EOI);

    _master_send_cmd(CMD_EOI);
}
