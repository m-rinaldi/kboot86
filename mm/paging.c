#include <paging.h>

#include <page_dir.h>
#include <page_tables.h>

#include <stddef.h>

#define MAX_PADDR   0x00efffff

// XXX
#include <kstdio.h>

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

    for (addr = 0; addr + PAGE_SIZE-1 <= MAX_PADDR; addr += PAGE_SIZE)
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

    // map the last page dir entry to itself
    if (page_dir_automap())
        return 1;

    return 0;
}

// TODO implemente this functions to set/get the CR3
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
    if (paddr + PAGE_SIZE-1 > MAX_PADDR)
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

// for the currently working page directory
// the page directory may be other than the local structure
uint32_t paging_vaddr2paddr(uint32_t vaddr)
{
    /*
        vaddr  = < pd_idx, pt_idx, phy >
        
        from the given vaddr we build another vaddr consisiting of:
        vaddr' = < 0xffc,  pd_idx, pt_idx >
    */
    uint32_t *pd, *pt;
    uint_fast16_t pd_idx = vaddr >> 22;
    uint_fast16_t pt_idx = vaddr >> 12 & 0x3ff;

    pd = (uint32_t *) (0xffc00000 | 0x3ff << 12);
    if (!(pd[pd_idx] & 1))
        return 0; // corresponding page directory entry not present

    pt = (uint32_t *) (0xffc00000 | (uint32_t) pd_idx << 12);
    if (!(pt[pt_idx] & 1))
        return 0; // page table entry not present
        
    return (uint32_t) ((pt[pt_idx] & ~0xfff) | (vaddr & 0xfff));    
}
