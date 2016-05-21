#include <timers.h>

#include <intr.h>
#include <eflags.h>

#include <stddef.h>

// head of the doubly linked list
static timer_t *_head = NULL;

void timers_init(void)
{
    _head = NULL;
}

void timers_add_timer(timer_t *tmr)
{
    bool intr_flag;

    intr_flag = eflags_get_intr_flag();
    intr_disable();
    { 
        tmr->next = _head;
        if (_head)
            _head->prev = tmr;
        _head = tmr;
    }
    eflags_set_intr_flag(intr_flag);
}

void timers_remove_timer(timer_t *tmr)
{
    bool intr_flag;
    
    intr_flag = eflags_get_intr_flag();
    intr_disable();
    {
        if (tmr->prev)
            tmr->prev->next = tmr->next;

        if (tmr->next)
            tmr->next->prev = tmr->prev;

        if (tmr == _head)
            _head = tmr->next;
    }
    eflags_set_intr_flag(intr_flag);

    tmr->prev = tmr->next = NULL; 
}

// to be called by the PIT's ISR
void timers_update(void)
{
    timer_t *tmr;

    for (tmr = _head; tmr; tmr = tmr->next) {
        if (!--tmr->counter)
            timers_remove_timer(tmr);  
    }  
}
