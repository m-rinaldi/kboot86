#pragma once

#include <stdbool.h>
#include <stdint.h>

static inline bool _is_4k_aligned_addr(uint32_t addr)
{
    return !(addr & 0x3ff);
}
