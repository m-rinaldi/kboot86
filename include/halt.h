#pragma once

static inline
void halt(void)
{
    asm volatile ("hlt\n\t" : : : "memory");
}
