#ifndef _INTR_H_
#define _INTR_H_

static inline
void enable_intr(void)
{
	asm volatile ("sti");
}

static inline
void disable_intr(void)
{
	asm volatile ("cli");
}

#endif
