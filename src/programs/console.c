#include "programs/console.h"
#include "conio.h"
#include "programs/mouse.h"
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define CMD_BUFFER_SIZE 128
#define PROMPT "> "

#define CMD_ECHO "echo"
#define CMD_ECHO_PREFIX "echo "
#define CMD_ECHO_PREFIX_LEN (sizeof(CMD_ECHO_PREFIX) - 1)

#define CMD_CLEAR "clear"
#define CMD_CLS "cls"

#define ASCII_PRINTABLE_MIN ' '
#define ASCII_PRINTABLE_MAX '~'
#define KEY_NONE ((int8_t)-1)

static char g_cmd_buffer[CMD_BUFFER_SIZE];
static uint16_t g_cmd_len = 0;
static bool g_console_initialized = false;

static void print_prompt(void) { cputs(PROMPT); }

static void execute_command(const char *cmd) {
  if (cmd[0] == '\0') {
    return;
  }

  if (strncmp(cmd, CMD_ECHO_PREFIX, CMD_ECHO_PREFIX_LEN) == 0) {
    cputs(&cmd[CMD_ECHO_PREFIX_LEN]);
    cputs("\n");
  } else if (strcmp(cmd, CMD_ECHO) == 0) {
    cputs("\n");
  } else if (strcmp(cmd, CMD_CLEAR) == 0 || strcmp(cmd, CMD_CLS) == 0) {
    clrscr();
  } else {
    cputs("Unknown command: ");
    cputs(cmd);
    cputs("\n");
  }
}

void CONSOLE_run(bool keyboard_changed, bool mouse_changed) {
  if (!g_console_initialized) {
    print_prompt();
    g_console_initialized = true;
  }

  if (kbhit()) {
    int8_t c = getch();

    if (c != KEY_NONE) {
      if (c == '\n' || c == '\r') {
        putch('\n');
        g_cmd_buffer[g_cmd_len] = '\0';
        execute_command(g_cmd_buffer);
        g_cmd_len = 0;
        print_prompt();
      } else if (c == '\b') {
        if (g_cmd_len > 0) {
          g_cmd_len--;
          putch('\b');
        }
      } else if (c >= ASCII_PRINTABLE_MIN && c <= ASCII_PRINTABLE_MAX) {
        if (g_cmd_len < (CMD_BUFFER_SIZE - 1)) {
          g_cmd_buffer[g_cmd_len++] = c;
          putch(c);
        }
      }
    }
  }

  if (mouse_changed) {
    render_console();
    MOUSE_run();
  }
}
