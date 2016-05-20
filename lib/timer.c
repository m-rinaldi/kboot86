#include <timer.h>

#include <timers.h>

#include <stddef.h>

void timer_start(timer_t *t, unsigned int timeout)
{
    t->prev = t->next = NULL;
    
    if (!(t->counter = t->init_count = timeout))
        return;

    timers_add_timer(t);
}

void timer_end(timer_t *t)
{
    timers_remove_timer(t);
}

void timer_restart(timer_t *t)
{
    // TODO
    (void) t;
}

bool timer_is_triggered(const timer_t *t)
{
    return !t->counter;
}
