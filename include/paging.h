#pragma once

#include <mm_common.h>
#include <stdint.h>

int paging_map(uint32_t vaddr, uint32_t paddr);
int paging_init(void);
void paging_enable(void);
