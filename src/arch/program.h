#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdbool.h>

typedef struct {
  void (*init)();
  void (*run)(bool keyboard_changed, bool mouse_changed);
} Program;

void set_current_program(Program p);
void init_program(void);
void run_program(void);

#endif
