#include "programs/console.h"
#include "conio.h"
#include "drivers/graphics.h"
#include "programs/mouse.h"

void CONSOLE_run(bool keyboard_changed, bool mouse_changed) {
  if (keyboard_changed && kbhit()) {
    getche();
  }

  if (keyboard_changed || mouse_changed) {
    clear(get_console_state()->current_background_color);
    render_console();
    MOUSE_run();
  }
}
