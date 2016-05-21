#include <timer.h>

#include <timer_list.h>
#include <eflags.h>
#include <intr.h>

#include <stddef.h>

/*
 timers have two states:
    1) triggered        <-->    counter is zero    --> timer not in timer list
    2) not triggered    <-->    counter not zero   <-- timer in timer list
 */

void timer_start(timer_t *tmr, unsigned int timeout)
{
    tmr->prev = tmr->next = NULL;
    tmr->in_list = false;
   
    tmr->counter = tmr->initial_counter = timeout;
    timer_list_add_timer(tmr);
}

void timer_end(timer_t *tmr)
{
    timer_list_remove_timer(tmr);
}

void timer_stop(timer_t *tmr)
{
    timer_list_remove_timer(tmr);
}

void timer_pause(timer_t *tmr)
{
    timer_list_remove_timer(tmr);
}

void timer_resume(timer_t *tmr)
{
    if (tmr->counter)
        timer_list_add_timer(tmr);
}

void timer_restart(timer_t *tmr)
{
    bool intr_flag;
    
    /*
     two scenarios:
        1) timer not in list    <--     timer already triggered
        2) timer in list        -->     timer not triggered yet
     */
  
    if (!tmr->in_list) { // 1st scenario
        tmr->counter = tmr->initial_counter;
        timer_list_add_timer(tmr);
        return;
    }

    intr_flag = eflags_get_intr_flag();
    intr_disable();
    {
        // check again whether the timer was removed from the list
        if (!tmr->in_list)
            timer_restart(tmr); // 1st scenario
        else // 2nd scenario
            tmr->counter = tmr->initial_counter;
    }
    eflags_set_intr_flag(intr_flag);
}

bool timer_is_triggered(const timer_t *tmr)
{
    return !tmr->counter;
}
