#include "arch/io.h"
#include "arch/program.h"
#include "drivers/graphics.h"
#include "programs/console.h"
#include "stdlib.h"
#include <conio.h>
#include <stdbool.h>
#include <stdint.h>

extern void setup_interrupts(void);

void kmain(uint32_t framebuffer, uint32_t width, uint32_t height, uint32_t bpp,
           uint32_t pitch) {
  init_graphics(framebuffer, width, height, bpp, pitch);
  init_console();
  setup_interrupts();

  volatile float a = 1.5f;
  volatile float b = 2.5f;
  volatile float result = 0.0f;
  cputs("Testing hardware x87 FPU...\n");
  // Inline x87 FPU addition
  __asm__ volatile("fld %1\n\t"   // Push 'a' onto FPU register stack
                   "fadd %2\n\t"  // Add 'b'
                   "fstps %0\n\t" // Store float result and pop FPU stack
                   : "=m"(result)
                   : "m"(a), "m"(b)
                   : "memory");
  if (result > 3.99f && result < 4.01f) {
    cputs("FPU Hardware Test: PASSED (1.5 + 2.5 = 4.0)\n");
  } else {
    cputs("FPU Hardware Test: FAILED\n");
  }

  init_program();
  set_current_program((Program){NULL, CONSOLE_run});

  while (1) {
    wait_for_interrupts();
    run_program();
  }
}
