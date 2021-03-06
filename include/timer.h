#pragma once

#include <stdbool.h>

typedef struct st_timer {
    unsigned int            initial_counter;

    volatile unsigned int   counter;
    volatile bool           in_list;

    struct st_timer         *prev;
    struct st_timer         *next;
} timer_t;


void timer_start(timer_t *, unsigned int);
void timer_end(timer_t *);
void timer_restart(timer_t *);
bool timer_is_triggered(const timer_t *);
