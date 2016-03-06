#include <page_dir.h>

#include <string.h>

typedef struct {
    unsigned int    present         :   1;
    unsigned int    zero1           :   3;
    /*
        unsigned int    read_write      :   1;
        unsigned int    user_supervisor :   1;
        unsigned int    write_through   :   1;
    */

    unsigned int    cache_disabled  :   1;
    /*
        unsigned int    accessed        :   1;
        unsigned int    zero            :   1;
        unsigned int    size            :   1;  
        unsigned int    ignored         :   1;
        unsigned int    available       :   3;
    */

    unsigned int    zero0           :   7;
    unsigned int    paddr           :   20; // page table 4-kB aligned addr
} __attribute__((packed)) page_dir_entry_t;

#define PAGE_DIR_TABLE_LEN  1024

// TODO volatile, does the MMU change the entries? Yes it does (access & dirty)
// the page directory table
// "aligned" variable attribute applies to the array, not to each element
static page_dir_entry_t _[PAGE_DIR_TABLE_LEN] __attribute__((aligned(4096)));

static inline bool _entry_num_is_valid(uint_fast16_t entry_num)
{
    return entry_num < PAGE_DIR_TABLE_LEN;
}

page_dir_entry_t page_dir_get_entry(uint_fast16_t entry_num)
{
    return _[entry_num & 0x3ff]; 
}

int page_dir_set_entry(uint_fast16_t entry_num, uint32_t paddr)
{
    if (!_entry_num_is_valid(entry_num))
        return 1;

    if (!_is_4k_aligned_addr(paddr))
        return 1;

    _[entry_num].paddr = paddr >> 12;
    _[entry_num].zero0 = 0;
    _[entry_num].cache_disabled = 1;
    _[entry_num].zero1 = 0;
    _[entry_num].present = 1;

    return 0;
}

bool page_dir_entry_is_present(uint_fast16_t entry_num)
{
    if (!_entry_num_is_valid(entry_num))
        return false;

    return page_dir_get_entry(entry_num).present; 
}

// load the paddr of the page directory in CR3
static inline void _load(page_dir_entry_t *page_dir_paddr)
{
    asm volatile (
                    "movl %0, %%cr3\n\t"
                    :   // no output
                    : "a" (page_dir_paddr)
                 );
}

// map the last page directory table entry to the page directory itself
int page_dir_automap(void)
{
    if (page_dir_set_entry(PAGE_DIR_TABLE_LEN-1, (uint32_t) &_[0]))
        return 1;

    return 0;
}

void page_dir_init(void)
{
    size_t i;

    // set all the entries in the page directory everything to zero
    for (i = 0; i < PAGE_DIR_TABLE_LEN; i++)
        bzero(_ + i, sizeof(_[0]));

    _load(_);
}
