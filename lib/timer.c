#include <timer.h>

#include <timers.h>
#include <eflags.h>
#include <intr.h>

#include <stddef.h>

void timer_start(timer_t *tmr, unsigned int timeout)
{
    tmr->prev = tmr->next = NULL;
   
    // timeout <- 0 actually means "timer disabled" 
    if (!(tmr->counter = tmr->initial_counter = timeout))
        return;

    timers_add_timer(tmr);
}

void timer_end(timer_t *tmr)
{
    timers_remove_timer(tmr);
}

void timer_stop(timer_t *tmr)
{
    // TODO
    (void) tmr;
}

void timer_restart(timer_t *tmr)
{
    bool intr_flag;
    
    /*
     two scenarios:
        1) timer is already triggered
        2) timer has not been triggered yet        
     */

    if (timer_is_triggered(tmr)) { // 1st scenario
        // prevent it from adding a timer with a null counter
        if ((tmr->counter = tmr->initial_counter))
            timers_add_timer(tmr);
        return;
    }

    intr_flag = eflags_get_intr_flag();
    intr_disable();
    {
        // check again whether the timer was triggered
        if (timer_is_triggered(tmr))
            timer_restart(tmr); // 1st scenario
        else    // 2nd scenario
            tmr->counter = tmr->initial_counter;
    }
    eflags_set_intr_flag(intr_flag);
}

bool timer_is_triggered(const timer_t *tmr)
{
    return !tmr->counter;
}
