#pragma once

#include <stdint.h>

int mbr_init(void);
int mbr_get_partition_info(unsigned int, uint32_t *start, uint32_t *num_secs);
void mbr_display(void);

