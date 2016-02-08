#pragma once

#include <stddef.h>
#include <ktypes.h>

int fat16_init(unsigned int partition_num);
void fat16_display_vid(void);
void fat16_display_root(void);
void fat16_display_dir_entry(const char *name);

ssize_t fat16_get_file_size(const char *filename);
ssize_t fat16_load(const char *filename, void *buf, const size_t buf_size);

