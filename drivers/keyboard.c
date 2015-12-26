#include <keyboard.h>
#include <stdint.h>
#include <stdbool.h>
#include "keymap.h"
#include <intr.h>
#include <io.h>
#include <pic.h>

// XXX
#include <console.h>

#define KEYBOARD_IRQ_NUM    0x01

// ports
#define DATA_REG    0x60
#define CONTROL_REG 0x64

extern void _keyboard_isr(void);

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
    // TODO save IF

    _.shift = _.rshift = _.lshift = false;
    _.ctrl = false;

    // TODO perform a controller reset

    intr_register_irq(KEYBOARD_IRQ_NUM, _keyboard_isr);

    // TODO restore IF

    return 0;
}

void keyboard_isr(void)
{
    uint8_t scancode;
    bool make;

    scancode = _read_buf();

    // is this keystroke a make or a break?
    make = !(0x80 & scancode);

    // clear the most significant bit (break bit)
    scancode = 0x7f & scancode;
    
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
    
    _keymap[scancode];
    // XXX
    console_puts("< key pressed > ");

keyboard_isr_end:
    _.shift = _.lshift || _.rshift;
    pic_send_eoi(KEYBOARD_IRQ_NUM);
    return;   
}

