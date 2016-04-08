#include <vga.h>
#include <stdint.h>
#include <io.h>

#define BASE_ADDR               0xb8000

// XXX it affects the cursor as well
#define ATTR_GREEN_ON_BLACK     0x0a
#define ATTR_WHITE_ON_BLUE      0x1f

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

    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t) (0x00ff & idx));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t) (0x00ff & (idx >> 8)));
}

// TODO _hide_cursor()

void vga_scroll_down(void)
{
    unsigned int i;

    // TODO inline asm with movsw instead
    for (i = 0; i < VGA_NUM_COLS * VGA_NUM_ROWS - VGA_NUM_COLS; i++)
        _mem[i] = _mem[i+VGA_NUM_COLS]; 

    // clear the undermost row
    vga_clear_row(VGA_NUM_ROWS - 1);
}

void vga_clear_row(unsigned int y)
{
    unsigned int i;

    for (i = 0; i < VGA_NUM_COLS; i++)
        vga_writec_attr_xy(' ', ATTR_GREEN_ON_BLACK, i, y); 
}

void vga_clear(void)
{
    unsigned int y;

    for (y = 0; y < VGA_NUM_ROWS; y++)
        vga_clear_row(y);
}

// Blue Screen of Death
void vga_bsod(void)
{
    unsigned int i, j;
    unsigned int idx;

    for (i = 0; i < VGA_NUM_COLS; i++)
        for (j = 0; j < VGA_NUM_ROWS; j++) {
            idx = _xy2idx(i, j);
            _mem[idx] = _encode_char_attr(_mem[idx] & 0xff, ATTR_WHITE_ON_BLUE);
        }
}
