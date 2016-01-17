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

#define IBUF_SIZE   128
static struct {
    uint8_t             attr;

    volatile uint8_t    curs_x;
    volatile uint8_t    curs_y;

    volatile bool       line_completed;
    char                ibuf[IBUF_SIZE];
    volatile size_t     widx;
} _;

// line feed
static void _line_feed(void)
{
    _.curs_x = 0;
    if (VGA_NUM_ROWS == ++_.curs_y) {
        vga_scroll_down();

        _.curs_y = VGA_NUM_ROWS - 1;
    }
}

static inline
void _curs_forward(void)
{
    if (VGA_NUM_COLS == ++_.curs_x)
        _line_feed();
}

static inline
void _curs_backward(void)
{
    if (!_.curs_x) {
        _.curs_x = VGA_NUM_COLS - 1;
        _.curs_y--;
    } else
        _.curs_x--;
}

static inline
bool _ibuf_is_full(void)
{
    return IBUF_SIZE == _.widx;
}

static inline
bool _ibuf_is_empty(void)
{
    return !_.widx;
}

static inline
void _reset_ibuf(void)
{
    _.widx = 0;
    _.line_completed = false;
} 

int console_init(void)
{
    _.attr = ATTR_GREEN_ON_BLACK;
    _.curs_x = _.curs_y = 0;    

    _.line_completed = false;
    _.widx = 0;

    console_clear();
    return 0;
}

static int _del_ibuf(void);

// kill control character
static void _kill(void)
{
    while (!_del_ibuf())
        ;
}


// does update the position of the cursor
static inline
void _putc_attr(char c, uint8_t attr)
{
    vga_writec_attr_xy(c, attr, _.curs_x, _.curs_y);

    // update cursor 
    _curs_forward();

//XXX
#if 0
    if (VGA_NUM_COLS == ++_.curs_x)
        _line_feed();
#endif
}

// returns zero if char must not be echoed on the screen
static inline
int _putc(char c, bool display)
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
            // not a control character
            if (display)
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
        _putc(s[i], true);

    // update the cursor after writing the whole string
    vga_draw_cursor_xy(_.curs_x, _.curs_y);

    return i;
}

// called from keyboard ISR
void console_put_ibuf(char c)
{
    bool printable;

    if ('\n' == c)
        _.line_completed = true;

    printable = _putc(c, !_ibuf_is_full());

    if (printable && !_ibuf_is_full()) {
        _.ibuf[_.widx++] = c;
        vga_draw_cursor_xy(_.curs_x, _.curs_y);
    }
}

static int _del_ibuf(void)
{
    if (_.line_completed)
        return -1;

    if (_ibuf_is_empty())
        return -1;

    _.widx--;

    _curs_backward();
    _putc(' ', true);
    _curs_backward();

    vga_draw_cursor_xy(_.curs_x, _.curs_y);
    return 0;
}

//XXX
#include <intr.h>
int console_get_line(char *buf, size_t *buf_len)
{
    size_t len;

    if (!buf || !buf_len)
        return 1;

    if (!*buf_len)
        return 0;    
    
    {
    loop:
        // TODO do not disable the whole interrupts, only the keyboard
        // TODO keyboard_disable_irq() instead
        intr_disable();
        if (!_.line_completed) {
            // TODO keyboard_einable_irq() instead
            intr_enable();
            goto loop;
        }
    }

    // avoid a buffer overrun
    len = *buf_len < _.widx ? *buf_len : _.widx;
    *buf_len = len;

    memcpy(buf, _.ibuf, len);

    _reset_ibuf();

    return 0;
}


