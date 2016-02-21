#include <page_table.h>

int page_table_set_entry(page_table_entry_t *pte, 
                         uint32_t paddr, unsigned int flags)
{
    if (!_is_4k_aligned_addr(paddr))
        return 1;   // not 4kB aligned addr

     

    return 0;
}


