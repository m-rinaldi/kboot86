#include <timers.h>

#include <intr.h>

#include <stddef.h>

// head of the doubly linked list
static timer_t *_head = NULL;

void timers_init(void)
{
    _head = NULL;
}

void timers_add_timer(timer_t *tmr)
{
    if (timer_is_triggered(tmr))
        return;
    
    // add timer to the head of the list
    intr_disable();
    { 
        tmr->next = _head;
        if (_head)
            _head->prev = tmr;
        _head = tmr;
    }
    intr_enable();
}

void timers_remove_timer(timer_t *tmr)
{

    intr_disable();
    {
        if (tmr->prev)
            tmr->prev->next = tmr->next;

        if (tmr->next)
            tmr->next->prev = tmr->prev;

        if (tmr == _head)
            _head = tmr->next;
    }
    intr_enable();

    tmr->prev = tmr->next = NULL; 
}

void timers_update(void)
{
    timer_t *tmr;

    for (tmr = _head; tmr; tmr = tmr->next) {
        if (!--tmr->counter)
            timers_remove_timer(tmr);  
    }  
}
