#include "arch/io.h"
#include "arch/program.h"
#include "drivers/graphics.h"
#include "programs/console.h"
#include "programs/mouse.h"
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

  init_program();
  set_current_program((Program){NULL, CONSOLE_run});

  while (1) {
    wait_for_interrupts();
    MOUSE_restore_bg();
    run_program();
    MOUSE_run();
    swap_buffers();
  }
}
