#include <paging.h>

#include <page_dir.h>
#include <page_table.h>

#define     MAX_PADDR   0x00efffff

static int _setup_identity(void)
{
    uint32_t addr;

    for (addr = 0; addr <= MAX_PADDR; addr += 4096) {
        if (addr % 1 << 22) { // set up a new entry in the page directory
            size_t pd_idx;
          
            pd_idx = addr / 1 << 22;
            page_dir_set_entry(pd_idx, addr);
        }

        // set up a new entry in the page table
        // TODO 
    }

    return 0;
}

int paging_init(void)
{
    page_dir_init();

    if (_setup_identity())
        return 1;

    return 0;
}

int paging_map(uint32_t vaddr, uint32_t paddr, unsigned int flags)
{
    if (!_is_4k_aligned_addr(vaddr) || !_is_4k_aligned_addr(paddr))
        return 1;

    // out of for use available physical memory
    if (paddr >= MAX_PADDR)
        return 1;

    // TODO

    return 0;
}
