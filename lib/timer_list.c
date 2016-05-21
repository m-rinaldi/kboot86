/*******************************************************************************
 * List of Timers
 ******************************************************************************/
#include <timer_list.h>

#include <intr.h>
#include <eflags.h>

#include <stddef.h>

// head of the doubly linked list
static timer_t *_head = NULL;

void timer_list_init(void)
{
    _head = NULL;
}

void timer_list_add_timer(timer_t *tmr)
{
    bool intr_flag;

    // timeout <- 0 actually means "timer disabled"
    // prevent the list from adding a timer with a null counter
    if (!tmr->counter)
        return;

    intr_flag = eflags_get_intr_flag();
    intr_disable();
    { 
        tmr->next = _head;
        if (_head)
            _head->prev = tmr;
        _head = tmr;

        // a pit interrupt that removes this timer from the list could happen
        // between sti and setting <in_list> to true
        // set <in_list> before enabling interrupts to avoid race conditions
        tmr->in_list = true;
    }
    eflags_set_intr_flag(intr_flag);
}

void timer_list_remove_timer(timer_t *tmr)
{
    bool intr_flag;

    if (!tmr->in_list)
        return;
    
    intr_flag = eflags_get_intr_flag();
    intr_disable();
    {
        if (tmr->prev)
            tmr->prev->next = tmr->next;

        if (tmr->next)
            tmr->next->prev = tmr->prev;

        if (tmr == _head)
            _head = tmr->next;
       
        // a ISR may reschedule this timer
        // set <in_list> here to avoid race conditions 
        tmr->in_list = false;
    }
    eflags_set_intr_flag(intr_flag);

    tmr->prev = tmr->next = NULL; 
}

// to be called by the PIT's ISR
void timer_list_update(void)
{
    timer_t *tmr;

    for (tmr = _head; tmr; tmr = tmr->next) {
        if (!--tmr->counter)
            timer_list_remove_timer(tmr);  
    }  
}
