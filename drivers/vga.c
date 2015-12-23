#include <vga.h>
#include <stdint.h>
#include <x86_io.h>

#define BASE_ADDR               0xb8000

#define ATTR_BLACK_ON_BLACK     0x00

static volatile uint16_t * const _mem = (uint16_t *) BASE_ADDR;
    
int vga_init(void)
{
    return 0;
}

// <column, row> -> index
static inline
uint16_t _xy2idx(unsigned int col, unsigned int row)
{
    return row * VGA_NUM_COLS + col;
}

static inline
uint16_t _encode_char_attr(char c, uint8_t attr)
{
    return (uint16_t) attr << 8 | c;
}

void vga_writec_attr_xy(char c, uint8_t attr,
                         unsigned int x, unsigned int y)
{
    _mem[_xy2idx(x, y)] = _encode_char_attr(c, attr);
}

void vga_draw_cursor_xy(unsigned int x, unsigned int y)
{
    uint16_t idx = _xy2idx(x, y);

    x86_outb(0x3d4, 0x0f);
    x86_outb(0x3d5, (uint8_t) (0x00ff & idx));
    x86_outb(0x3d4, 0x0e);
    x86_outb(0x3d5, (uint8_t) ((0xff00 & idx) >> 8));
}

// TODO _hide_cursor()

void vga_scroll_down(void)
{
    unsigned int i;

    // TODO inline asm with movsw instead
    for (i = 0; i < VGA_NUM_COLS * VGA_NUM_ROWS - VGA_NUM_COLS; i++)
        _mem[i] = _mem[i+VGA_NUM_COLS]; 

    // clear the undermost row
    for (i = 0; i < VGA_NUM_COLS; i++)
        vga_writec_attr_xy(' ', ATTR_BLACK_ON_BLACK, i, VGA_NUM_ROWS - 1);
}

void vga_clear_row(unsigned int y)
{
    unsigned int i;

    for (i = 0; i < VGA_NUM_COLS; i++)
        vga_writec_attr_xy(' ', ATTR_BLACK_ON_BLACK, i, y); 
}

void vga_clear(void)
{
    unsigned int x, y;

    // TODO  check that the whole screen is being written by changing the
    //       background color
    for (y = 0; y < VGA_NUM_ROWS; y++)
        for (x = 0; x < VGA_NUM_COLS; x++)
            vga_writec_attr_xy('X', ATTR_BLACK_ON_BLACK, x, y);
}
