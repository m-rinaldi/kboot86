#include <paging.h>

#include <page_dir.h>
#include <page_tables.h>

#include <stddef.h>

#define MAX_PADDR   0x00efffff
#define PAGE_SIZE   4096

static inline
uint_fast16_t _vaddr2pd_idx(uint32_t vaddr)
{
    return vaddr >> 22;
}

static inline
uint_fast16_t _vaddr2pt_idx(uint32_t vaddr)
{
    return vaddr >> 12 & 0x3ff;
}

static int _setup_identity(void)
{
    uint32_t addr;

    for (addr = 0; addr <= MAX_PADDR + PAGE_SIZE; addr += PAGE_SIZE)
        if (paging_map(addr, addr))
            return 1;

    return 0;
}

int paging_init(void)
{
    page_dir_init();
    page_tables_init();

    // TODO if paging is already enabled, copy that setup being used
    if (_setup_identity())
        return 1;

    return 0;
}

// TODO uint32_t _get_page_dir()
// TODO uint32_t _set_page_dir()

static inline void _reset_mmu_cache(void)
{
    asm volatile
        (
            "# _reset_mmu_cache()\n\t"
            "movl %%cr3, %%eax\n\t"
            "movl %%eax, %%cr3\n\t" 
            :   // no output
            :   // no input
            :   "eax"
        );
}

int paging_map(uint32_t vaddr, uint32_t paddr)
{
    uint_fast16_t pd_idx;
    uint_fast16_t pt_idx;    

    if (!_is_4k_aligned_addr(vaddr) || !_is_4k_aligned_addr(paddr))
        return 1;

    // out of for-use-available physical memory
    if (paddr + PAGE_SIZE > MAX_PADDR)
        return 1;

    // localize the entry in the page directory
    pd_idx = _vaddr2pd_idx(vaddr);

    // localize the entry in the pointed page table
    pt_idx = _vaddr2pt_idx(vaddr);

    if (!page_dir_entry_is_present(pd_idx)) {
        // set the corresponding entry in the page directory
        uint32_t pt_paddr;

        // get the paddr the page table this page dir entry will point at
        // TODO what if no identity paging?
        if (!(pt_paddr = page_tables_get_paddr(pd_idx)))
            return 1;
        
        if (page_dir_set_entry(pd_idx, pt_paddr))
            return 1;
    }

    if (page_tables_set_entry(pd_idx, pt_idx, paddr))
        return 1;

    _reset_mmu_cache();

    return 0;
}
