#pragma once

#include <var_entry.h>

void var_table_init(void);

int var_set_entry(const token_t *var_name, const token_t *val);
var_entry_t *var_get_entry(const token_t *);
int var_foreach(int (*do_func)(var_entry_t *ve));
