#pragma once

#include <mm_common.h>
#include <stdint.h>

#define PAGE_SIZE  4096

int paging_map(uint32_t vaddr, uint32_t paddr, bool writable);
int paging_unmap(uint32_t vaddr);
int paging_init(void);
void paging_enable(void);
uint32_t paging_vaddr2paddr(uint32_t vaddr);
