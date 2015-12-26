/*
    PIT: Programmable Interval Timer
    
    The PIT chip has 3 channes: CH0, CH1 & CH2. 
    We are only concerned about CH0
 */
#include <intr.h>
#include <io.h>
#include <eflags.h>
#include <pic.h>
#include <string.h>

//XXX
#include <console.h>

#define PIT_IRQ_NUM 0x00

// ports
#define CMD         0x43
#define DATA        0x40

typedef struct {
    unsigned int    bcd     :   1;
    unsigned int    mode    :   3;
    unsigned int    rw      :   2;
    unsigned int    cntr    :   2;
} __attribute__((packed)) cmd_reg_t;

#define BCD0    0   // 16-bit counter
#define BCD1    1   // 4x BCD decade counter

#define MODE0   0   // interrupt on terminal count
#define MODE1   1   // hardware retriggerable one shot 
#define MODE2   2   // rate generator
#define MODE3   3   // square wave mode
#define MODE4   4   // software strobe
#define MODE5   5   // hardware strobe

#define RW1     1   // LSB
#define RW2     2   // MSB
#define RW3     3   // LSB then MSB

#define CNTR0   0   // counter #0
#define CNTR1   1   // counter #1
#define CNTR2   2   // counter #2


#define FREQ_HZ_MAX     1193180
#define DIV_MAX         0xffff
#define FREQ_HZ_MIN     (FREQ_HZ_MAX/DIV_MAX) 

static uint32_t _num_ticks;

extern void _pit_isr(void);       /* The ASM wrapper for the pit ISR */

static inline
void _set_cmd_reg(cmd_reg_t reg)
{
    uint8_t _reg;

    memcpy(&_reg, &reg, sizeof(_reg));
    outb(CMD, _reg);   
}

static inline
int _set_freq_hz(uint32_t freq_hz)
{
    cmd_reg_t reg;
    uint16_t div;

    if (!freq_hz || freq_hz > FREQ_HZ_MAX || freq_hz < FREQ_HZ_MIN)
        return 1;
    
    div = FREQ_HZ_MAX / freq_hz;

    reg.bcd     = BCD0;
    reg.mode    = MODE3;
    reg.rw      = RW3;
    reg.cntr    = CNTR0;

    _set_cmd_reg(reg);

    // LSB first, then MSB
    outb(DATA, 0xff & div);
    outb(DATA, (div >> 8) & 0xff);

    return 0;
}

int pit_init(void)
{
    bool IF;
    bool ret = false;

    // save IF
    IF = eflags_get_IF();
    intr_disable();

    _num_ticks = 0;
    intr_register_irq(PIT_IRQ_NUM, _pit_isr);

    if (_set_freq_hz(100))
        ret = true;

    // restore IF
    eflags_set_IF(IF);

    return ret;
}


void pit_isr(void)
{
    _num_ticks++;
    pic_send_eoi(PIT_IRQ_NUM);
    return;     
}

