#pragma once

#include <timer.h>

void timers_init(void);
void timers_add_timer(timer_t *t);
void timers_remove_timer(timer_t *);
void timers_update(void);
