#include "programs/console.h"
#include "conio.h"
#include "programs/mouse.h"

void CONSOLE_run(bool keyboard_changed, bool mouse_changed) {
  if (kbhit()) {
    getche();
  }

  if (mouse_changed) {
    render_console();
    MOUSE_run();
  }
}
