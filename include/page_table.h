#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PAGE_TABLE_LEN  1024
typedef struct {
    unsigned int    addr            :   20;     // page table 4-kB aligned addr
    unsigned int    available       :   3;
    unsigned int    global          :   1;
    unsigned int    zero            :   1;
    unsigned int    dirty           :   1;  
    unsigned int    accessed        :   1;
    unsigned int    cache_disabled  :   1;
    unsigned int    write_through   :   1;
    unsigned int    user_supervisor :   1;
    unsigned int    read_write      :   1;
    unsigned int    present         :   1;
} __attribute__((packed)) page_table_entry_t;


static inline bool _is_4k_aligned_addr(uint32_t addr)
{
    return !(addr & 0x3ff);
}
