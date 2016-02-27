#pragma once

#include <mm_common.h>
#include <stdint.h>

#define PAGE_SIZE  4096

int paging_map(uint32_t vaddr, uint32_t paddr);
int paging_init(void);
void paging_enable(void);
