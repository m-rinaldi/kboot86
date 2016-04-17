#pragma once

#include <stdbool.h>
#include <stdint.h>

#define PADDR_MIN           (1  << 20)              //  1 MiB
#define PADDR_MAX           ((15 << 20) - 1)        // 15 MiB - 1 B

#define KERNEL_MEMSZ_MAX    (1 << 20)               // 1 MiB

#define KERNEL_PADDR_MIN    PADDR_MIN
#define KERNEL_PADDR_MAX    PADDR_MAX

#define KERNEL_VADDR_MIN    (1 << 20)               //  1 MiB
// last page table entry used for poiting at itself (highest 4 MiB not usable)
#define KERNEL_VADDR_MAX    ((0x3ffU << 22) - 1)    //  0xffc00000

static inline bool _is_addr_page_aligned(uint32_t addr)
{
    return !(addr & 0x3ff);
}
