#include <keyboard.h>
#include "keymap.h"

#include <intr.h>
#include <io.h>
#include <pic.h>
#include <console.h>
#include <eflags.h>

#include <stdint.h>
#include <stdbool.h>

// XXX
#include <kstdio.h>

#define KEYBOARD_IRQ_NUM    0x01

// ports
#define DATA_REG    0x60
#define CONTROL_REG 0x64

// keyboard's state
static struct {
    bool shift, rshift, lshift;
    bool ctrl;
} _;

static inline
uint8_t _read_buf(void)
{
    return inb(DATA_REG);
}

int keyboard_init(void)
{
    extern void _keyboard_isr(void);
    bool intr_flag;

    _.shift = _.rshift = _.lshift = false;
    _.ctrl = false;
    
    // save IF
    intr_flag = eflags_get_intr_flag();
    intr_disable();

    // TODO perform a controller reset

    intr_register_irq(KEYBOARD_IRQ_NUM, _keyboard_isr);
    
    pic_send_eoi(KEYBOARD_IRQ_NUM);
    keyboard_enable_irq();

    // restore previous IF
    eflags_set_intr_flag(intr_flag);

    return 0;
}

void keyboard_enable_irq(void)
{
    pic_enable_irq(KEYBOARD_IRQ_NUM);
}

void keyboard_disable_irq(void)
{
    pic_disable_irq(KEYBOARD_IRQ_NUM);
}

void keyboard_isr(void)
{
    uint8_t scancode;
    bool make;

    scancode = _read_buf();

    // is this keystroke a make or a break?
    make = !(0x80 & scancode);

    // clear the break bit
    scancode &= 0x7f;

    switch (scancode) {
        case LCTRL:
            _.ctrl = make;
            goto keyboard_isr_end;

        case LSHIFT:
            _.lshift = make;
            goto keyboard_isr_end;

        case RSHIFT:
            _.rshift = make;      
            goto keyboard_isr_end;
    }

    if (!make)
        goto keyboard_isr_end;
   
    {
        char c;

        // map control characters 
        switch (c = _keymap[scancode][!!_.shift]) {
            case 'u':
                if (_.ctrl)
                    c = '\r';   // line kill
                break;
    
            case 'l':
                if (_.ctrl)
                    c = 127;
                break;

            case 127:   // DEL -> backspace
                c = '\b';       // erase
                break;
        }

        if (c)
            console_put_ibuf(c);
    }

keyboard_isr_end:
    _.shift = _.lshift || _.rshift;
    pic_send_eoi(KEYBOARD_IRQ_NUM);
    return;   
}
