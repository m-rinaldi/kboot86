// TODO tabs were 3 and they shold be 4
#include <monitor.h>
#include <stdint.h>

#define BASE_ADDR          0xb8000
#define NUM_COLS           80
#define NUM_ROWS           25


// attributes
#define ATTR_BLACK_ON_BLACK      0x00
#define ATTR_LGREY_ON_BLACK      0x07
#define ATTR_GREEN_ON_BLACK      0x0a
#define ATTR_WHITE_ON_BLUE       0x1f

static struct {
   volatile uint16_t * const mem;

   const uint8_t  attr;

   uint8_t  curs_x;
   uint8_t  curs_y;
} _ = {(uint16_t *) BASE_ADDR, ATTR_GREEN_ON_BLACK, 0, 0};

static void _scroll_down(void);


int monitor_init(void)
{
   _.curs_x = _.curs_y = 0;
   monitor_clear();
   return 0;
}

// <column, row> -> index
static inline
unsigned int _cr2idx(unsigned int col, unsigned int row)
{
   return row * NUM_COLS + col;
}

static inline
unsigned int _curs_idx(void)
{
   return _cr2idx(_.curs_x, _.curs_y);
}

static inline
uint16_t _encode_char_attr(char c, uint8_t attr)
{
   return (uint16_t) attr << 8 | c;
}

// does not update the position of the cursor
static inline
void _write_char_attr_cr(char c, uint8_t attr,
                         unsigned int col, unsigned int row)
{
   _.mem[_cr2idx(col, row)] = _encode_char_attr(c, attr);
}

// does update the position of the cursor
static inline
void _putc_attr(char c, uint8_t attr)
{
   _write_char_attr_cr(c, attr, _.curs_x, _.curs_y);

   // update cursor 
   _.curs_x = (_.curs_x + 1) % NUM_COLS;
   if (!_.curs_x && NUM_ROWS == ++_.curs_y)
      _scroll_down();
}

static inline
void _draw_cursor(void)
{
   unsigned int idx = _curs_idx();

   (void) idx;
   /* TODO
      set low index
      set high index
   */ 
}

static
void _scroll_down(void)
{
   unsigned int i;

   // TODO inline asm with movsw instead
   for (i = 0; i < NUM_COLS * NUM_ROWS - NUM_COLS; i++)
      _.mem[i] = _.mem[i+NUM_COLS]; 

   // clear the undermost row
   for (i = 0; i < NUM_COLS; i++)
      _write_char_attr_cr(' ', _.attr, i, NUM_ROWS - 1);

   // set cursor position
   _.curs_x = 0;
   _.curs_y = NUM_COLS - 1;
}

static inline
void _putc(char c)
{
   _putc_attr(c, _.attr);
}

void monitor_clear(void)
{
   int i;

   // TODO  check that the whole screen is being written by changing the
   //       background color
   for (i = 0; i < NUM_COLS * NUM_ROWS; i++)
      _putc_attr('X', ATTR_BLACK_ON_BLACK);
}

int monitor_puts(const char *s)
{
   int i;

   if (!s)
      return 0;

   for (i = 0; s[i]; i++)
      _putc(s[i]);
   _draw_cursor();

   return i;
}
