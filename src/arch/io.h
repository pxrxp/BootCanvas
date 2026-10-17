#ifndef IO_H
#define IO_H

#include <stdint.h>

static inline uint8_t inb(uint16_t port) {
  uint8_t ret;
  __asm__ volatile("inb %1, %0"
                   // Output
                   : "=a"(ret) // al, ax or eax (write-only)
                   // Input
                   : "Nd"(port) // constant or dx
  );
  return ret;
}

static inline void outb(uint16_t port, uint8_t val) {
  __asm__ volatile("outb %0, %1"
                   // Output
                   :
                   // Input
                   : "a"(val),    // al (because 8 bit)
                     "Nd"(port)); // constant or dx
}

static inline void wait_for_interrupts(void) { __asm__ volatile("hlt"); }

#endif
