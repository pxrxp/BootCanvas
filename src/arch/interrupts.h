#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

void remap_pic(void);
void end_of_interrupt(uint8_t irq);

#endif
