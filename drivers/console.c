#include <console.h>

#include <vga.h>
#include <keyboard.h>
#include <halt.h>

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// attributes
#define ATTR_BLACK_ON_BLACK     0x00
#define ATTR_LGREY_ON_BLACK     0x07
#define ATTR_GREEN_ON_BLACK     0x0a
#define ATTR_WHITE_ON_BLUE      0x1f
#define ATTR_RED_ON_BLUE        0x14    

#define IBUF_SIZE   128
#define TAB_WIDTH   4
static struct {
    uint8_t             attr;

    volatile uint8_t    curs_x;
    volatile uint8_t    curs_y;

    volatile bool       line_completed;
    char                ibuf[IBUF_SIZE];
    volatile size_t     widx;
    volatile bool       cleared;
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

static inline int _putc(char c, bool display);

static void _tab(void)
{
    unsigned int i;

    for (i = 0; i < TAB_WIDTH; i++)
        _putc(' ', true);
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

    if (keyboard_init())
        return 1;

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
}

// returns zero if char must not be echoed on the screen
static inline
int _putc(char c, bool display)
{
    switch (c) {
        case '\n':
            _line_feed();
            break;

        case '\t':
            _tab();
            break;

        case '\r':
            _kill();
            break;

        case 127:
            console_clear();
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
    _.cleared = true;
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

int console_puts_err(const char *s)
{
    uint8_t saved_attr;
    int ret;

    saved_attr = _.attr;

    _.attr = ATTR_RED_ON_BLUE;
    ret = console_puts(s);

    // restore attr
    _.attr = saved_attr;

    return ret;
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

    // TODO del tab
    _curs_backward();
    _putc(' ', true);
    _curs_backward();

    vga_draw_cursor_xy(_.curs_x, _.curs_y);
    return 0;
}

static void _display_ibuf(void)
{
    unsigned int i;

    for (i = 0; i < _.widx; i++)
        _putc(_.ibuf[i], true);

    vga_draw_cursor_xy(_.curs_x, _.curs_y);
}

int console_get_line(char *buf, size_t *buf_len)
{
    size_t len;

    if (_.cleared) { // screen just cleared
        // display the already-entered input again
        _display_ibuf();
        _.cleared = false;
    }

loop:
    keyboard_disable_irq();
    if (!_.line_completed) {
        if (_.cleared) {
            keyboard_enable_irq();
            return 1;   // return the control to get the prompt displayed
        }

        keyboard_enable_irq();
        halt();
        goto loop;
    }

    // avoid a buffer overrun
    len = *buf_len < _.widx ? *buf_len : _.widx;
    *buf_len = len;

    memcpy(buf, _.ibuf, len);

    _reset_ibuf();

    return 0;
}


