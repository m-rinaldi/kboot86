#pragma once

#include <stdint.h>

#define VGA_NUM_COLS    80
#define VGA_NUM_ROWS    25

int vga_init(void);
void vga_clear(void);
void vga_writec_attr_xy(char c, uint8_t attr, unsigned int x, unsigned int y);
void vga_draw_cursor_xy(unsigned int x, unsigned int y);
void vga_clear_row(unsigned int y);
void vga_scroll_down(void);
void vga_set_bsod(void);
