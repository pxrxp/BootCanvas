#include "util.h"

void delay(uint32_t seconds) {
  for (volatile uint32_t i = 0; i < seconds; i++)
    for (volatile uint32_t i = 0; i < 0x16EEEEEE; i++)
      ;
}
