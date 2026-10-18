#include "programs/console.h"
#include "conio.h"
#include "drivers/graphics.h"
#include "programs/mouse.h"

void CONSOLE_run(bool keyboard_changed, bool mouse_changed) {
  if (kbhit()) {
    getche();
  }

  if (kbhit() || mouse_changed) {
    clear(get_console_state()->current_background_color);
    render_console();
    MOUSE_run();
  }
}
