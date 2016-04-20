#ifndef _EFLAGS_H_
#define _EFLAGS_H_

#include <stdbool.h>

bool eflags_get_intr_flag(void);
void eflags_set_intr_flag(bool);

#endif // _EFLAGS_H_
