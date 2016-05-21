#pragma once

#include <timer.h>

void timer_list_init(void);
void timer_list_add_timer(timer_t *t);
void timer_list_remove_timer(timer_t *);
void timer_list_update(void);
