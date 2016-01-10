#include <console.h>
#include <vga.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// attributes
#define ATTR_BLACK_ON_BLACK      0x00
#define ATTR_LGREY_ON_BLACK      0x07
#define ATTR_GREEN_ON_BLACK      0x0a
#define ATTR_WHITE_ON_BLUE       0x1f

#define IBUF_SIZE   32
static struct {
    uint8_t     attr;

    uint8_t     curs_x;
    uint8_t     curs_y;

    volatile bool   line_completed;
    char            ibuf[IBUF_SIZE];
    volatile size_t iidx;  
} _;

static inline
bool _ibuf_is_full(void)
{
    return IBUF_SIZE == _.iidx;
}

static inline
bool _ibuf_is_empty(void)
{
    return !_.iidx;
}

static inline
void _reset_ibuf(void)
{
    _.iidx = 0;
    _.line_completed = false;
} 

int console_init(void)
{
    _.attr = ATTR_GREEN_ON_BLACK;
    _.curs_x = _.curs_y = 0;    

    _.line_completed = false;
    _.iidx = 0;

    console_clear();
    return 0;
}

// line feed
static void _line_feed(void)
{
    _.curs_x = 0;
    if (VGA_NUM_ROWS <= ++_.curs_y) {
        vga_scroll_down();

        _.curs_y = VGA_NUM_COLS - 1;
    }
}

// kill control character
static void _kill(void)
{
    // TODO call _del_ibuf() until ibuf is empty 
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
        _.curs_y = VGA_NUM_ROWS - 1;
    }
}

static int _del_ibuf(void);

// returns zero if char must not be printed on the screen
static inline
int _putc(char c)
{
    switch (c) {
        case '\n':
            _line_feed();
            break;

        case '\r':
            _kill();
            break;

        case '\b':
            _del_ibuf();
            break;

        default:  
            _putc_attr(c, _.attr);
            return 1;
    }
    return 0;
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

    // update the cursor after writing the whole string
    vga_draw_cursor_xy(_.curs_x, _.curs_y);

    return i;
}

void console_put_ibuf(char c)
{
    if (_ibuf_is_full())
        return;

    if ('\n' == c)
        _.line_completed = true;

    if (_putc(c))
        _.ibuf[_.iidx++] = c;

    vga_draw_cursor_xy(_.curs_x, _.curs_y);
}

static int _del_ibuf(void)
{
    // TODO source for race conditions?

    if (_ibuf_is_empty())
        return -1;

    _.iidx--;

    // TODO update _.curs_x properly
    // TODO what if line wrap around (if _.curs_x = 0 before decrement)
    _.curs_x--;
    _putc(' ');
    _.curs_x--;

    vga_draw_cursor_xy(_.curs_x, _.curs_y);
    return 0;
}

int console_get_line(char *buf, size_t *buf_len)
{
    size_t len;

    if (!buf || !buf_len)
        return 1;

    if (!*buf_len)
        return 0;    

    while (!_.line_completed)
        ;

    len = *buf_len < _.iidx ? *buf_len : _.iidx;
    *buf_len = len;
    memcpy(buf, _.ibuf, len);

    _reset_ibuf();

    return 0;
}


