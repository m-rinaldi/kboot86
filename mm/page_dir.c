#include <page_dir.h>

#include <page_table.h>

#include <string.h>

typedef struct {
    unsigned int    addr            :   20;     // page table 4-kB aligned addr
    unsigned int    available       :   3;
    unsigned int    ignored         :   1;
    unsigned int    size            :   1;  
    unsigned int    zero            :   1;
    unsigned int    accessed        :   1;
    unsigned int    cache_disabled  :   1;
    unsigned int    write_through   :   1;
    unsigned int    user_supervisor :   1;
    unsigned int    read_write      :   1;
    unsigned int    present         :   1;
} __attribute__((packed)) page_dir_entry_t;

#define PAGE_DIR_TABLE_LEN  1024

// 15MB / 4MB => 4 tables
#define NUM_PAGE_TABLES     4

// TODO volatile, does the MMU change the entries?
// the page directory table
static page_dir_entry_t _[PAGE_DIR_TABLE_LEN] __attribute__((aligned(4096)));

// the lowest 1 MB page table for a 1:1 mapping
static
page_table_entry_t _table[NUM_PAGE_TABLES][PAGE_TABLE_LEN]
__attribute__((aligned(4096)));


page_dir_entry_t page_dir_get_entry(size_t entry_num)
{
    return _[entry_num & 0x3ff]; 
}

// TODO add more args
int page_dir_set_entry(size_t entry_num, uintptr_t paddr)
{
    if (entry_num >= PAGE_DIR_TABLE_LEN)
        return 1;

    // TODO
    (void) paddr;

    return 0;
}

void page_dir_init(void)
{
    size_t i;
    // set everything to zero

    for (i = 0; i < PAGE_DIR_TABLE_LEN; i++)
        bzero(_ + i, sizeof(_[0]));
}
