// TODO tabs were 3 and they shold be 4
#include <console.h>
#include <vga.h>

#include <stdint.h>

// attributes
#define ATTR_BLACK_ON_BLACK      0x00
#define ATTR_LGREY_ON_BLACK      0x07
#define ATTR_GREEN_ON_BLACK      0x0a
#define ATTR_WHITE_ON_BLUE       0x1f

static struct {
    const uint8_t  attr;

    uint8_t  curs_x;
    uint8_t  curs_y;
} _ = {ATTR_GREEN_ON_BLACK, 0, 0};

int console_init(void)
{
    console_clear();
    return 0;
}

// line feed
static void _lf(void)
{
    _.curs_x = 0;
    if (VGA_NUM_ROWS <= ++_.curs_y) {
        vga_scroll_down();

        _.curs_y = VGA_NUM_COLS - 1;
    }
}

// carriage return
static void _cr(void)
{
    _.curs_x = 0;
    vga_clear_row(_.curs_y);
}

// does update the position of the cursor
static inline
void _putc_attr(char c, uint8_t attr)
{
    vga_writec_attr_xy(c, attr, _.curs_x, _.curs_y);

    // update cursor 
    _.curs_x = (_.curs_x + 1) % VGA_NUM_COLS;
    if (!_.curs_x && VGA_NUM_ROWS <= ++_.curs_y) {
        vga_scroll_down();

        // set cursor position
        _.curs_x = 0;
        _.curs_y = VGA_NUM_COLS - 1;
    }
}

static inline
void _putc(char c)
{
    switch (c) {
        case '\n':
            _lf();
            break;

        case '\r':
            _cr();
            break;

        default:  
            _putc_attr(c, _.attr);
    }
}

void console_clear(void)
{
    vga_clear();
    _.curs_x = _.curs_y = 0;
}


int console_puts(const char *s)
{
    int i;

    if (!s)
        return 0;

    for (i = 0; s[i]; i++)
        _putc(s[i]);
    vga_draw_cursor_xy(_.curs_x, _.curs_y);

    return i;
}
