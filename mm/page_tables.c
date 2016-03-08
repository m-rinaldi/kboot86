#include <page_tables.h>

#include <stddef.h>
#include <string.h>

#define PAGE_TABLE_LEN  1024
typedef struct {
    unsigned int    present         :   1;
    unsigned int    read_write      :   1;
    unsigned int    user            :   1;
    unsigned int    write_through   :   1;
    unsigned int    cache_disabled  :   1;
    unsigned int    zero            :   7;
    /*
        unsigned int    accessed        :   1;
        unsigned int    dirty           :   1;
        unsigned int    zero            :   1;
        unsigned int    global          :   1;
        unsigned int    available       :   3;
    */
    unsigned int    paddr           :   20; // page table 4-kB aligned addr
} __attribute__((packed)) page_table_entry_t;

typedef page_table_entry_t page_table_t[PAGE_TABLE_LEN];

// each page table can map up to 4MB
#define NUM_PAGE_TABLES 5
static page_table_t _[NUM_PAGE_TABLES] __attribute__((aligned(4096)));

/*
    only NUM_PAGE_TABLES page tables are implemented, the last page table
    is used only for the kernel and it can only map it to 0xc0000000
*/

static inline bool _is_table_idx_implemented(uint_fast16_t table_idx)
{
    if (table_idx < NUM_PAGE_TABLES-1  || 768 == table_idx)
        return true;

    return false;
}

static inline uint_fast8_t _table_idx2array_idx(uint_fast16_t table_idx)
{
    if (table_idx < NUM_PAGE_TABLES-1)
        return table_idx;

    return NUM_PAGE_TABLES-1;
}

static void _init_entry(page_table_entry_t *entry)
{
    bzero(entry, sizeof(*entry));
}

static void _init(page_table_t *pt)
{
    size_t i;

    for (i = 0; i < PAGE_TABLE_LEN; i++)
        _init_entry(&i[(page_table_entry_t *) pt]);   
}

void page_tables_init(void)
{
    size_t i;

    for (i = 0; i < NUM_PAGE_TABLES; i++)
        _init(_ + i);
}

uint32_t page_tables_get_paddr(uint_fast16_t table_idx)
{
    uint_fast8_t array_idx;
    page_table_t *pt;
    
    if (!_is_table_idx_implemented(table_idx))
        return 1;

    array_idx = _table_idx2array_idx(table_idx);
    pt = &_[array_idx];

    return (uint32_t) pt;
}

int page_tables_set_entry(uint_fast16_t table_idx, uint_fast16_t entry_num, 
                          uint32_t paddr)
{
    uint_fast16_t array_idx;
    page_table_entry_t *pte;

    if (!_is_4k_aligned_addr(paddr))
        return 1;   // not page-aligned addr

    if (!_is_table_idx_implemented(table_idx))
        return 1;

    array_idx = _table_idx2array_idx(table_idx);

    pte = &_[array_idx][entry_num];
    pte->paddr = paddr >> 12;
    pte->zero = 0;
    pte->cache_disabled = 1;
    pte->user = 0;
    pte->present = 1;

    return 0;
}

int page_tables_clear_entry(uint_fast16_t table_idx, uint_fast16_t entry_num)
{
    uint_fast16_t array_idx;
    page_table_entry_t *pte;

    if (!_is_table_idx_implemented(table_idx))
        return 1;
    
    array_idx = _table_idx2array_idx(table_idx);
    pte = &_[array_idx][entry_num];
    _init_entry(pte);

    return 0;
}
