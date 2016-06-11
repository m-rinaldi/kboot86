#pragma once

#include <mm_common.h>

#include <stdbool.h>
#include <stdint.h>

void page_tables_init(void);
uint32_t page_tables_get_paddr(uint_fast16_t);
bool page_tables_entry_is_present(uint_fast16_t, uint_fast16_t);
int page_tables_set_entry(uint_fast16_t, uint_fast16_t, uint32_t, bool);
int page_tables_clear_entry(uint_fast16_t, uint_fast16_t);
