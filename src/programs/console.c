#include "programs/console.h"
#include "arch/program.h"
#include "conio.h"
#include "ctype.h"
#include "drivers/graphics.h"
#include "programs/calc_gui.h"
#include "programs/paint_gui.h"
#include "stdlib.h"
#include "string.h"

#define CMD_BUFFER_SIZE 128
#define PROMPT "> "

#define CMD_ECHO "echo"
#define CMD_ECHO_PREFIX "echo "
#define CMD_ECHO_PREFIX_LEN (sizeof(CMD_ECHO_PREFIX) - 1)

#define CMD_CLEAR "clear"
#define CMD_CLS "cls"
#define CMD_HELP "help"
#define CMD_ABOUT "about"

#define CMD_PAINT "paint"
#define CMD_MSPAINT "mspaint"
#define CMD_DRAW "draw"

#define CMD_CALC "calc"
#define CMD_CALCULATOR "calculator"

#define ASCII_PRINTABLE_MIN ' '
#define ASCII_PRINTABLE_MAX '~'
#define KEY_NONE ((int8_t)-1)

static char g_cmd_buffer[CMD_BUFFER_SIZE];
static uint16_t g_cmd_len = 0;
static bool g_console_initialized = false;

static void print_prompt(void) { cputs(PROMPT); }

static const char *trim_leading_spaces(const char *str) {
  while (*str && isspace((unsigned char)*str)) {
    str++;
  }
  return str;
}

static void execute_command(const char *cmd_line) {
  cmd_line = trim_leading_spaces(cmd_line);
  if (*cmd_line == '\0') {
    return;
  }

  // Command: echo
  if (strncmp(cmd_line, CMD_ECHO_PREFIX, CMD_ECHO_PREFIX_LEN) == 0) {
    const char *arg = trim_leading_spaces(cmd_line + CMD_ECHO_PREFIX_LEN);
    cputs(arg);
    cputs("\n");
  } else if (strcmp(cmd_line, CMD_ECHO) == 0) {
    cputs("\n");
  }
  // Command: clear / cls
  else if (strcmp(cmd_line, CMD_CLEAR) == 0 || strcmp(cmd_line, CMD_CLS) == 0) {
    clrscr();
  }
  // Command: help
  else if (strcmp(cmd_line, CMD_HELP) == 0) {
    cputs("Available Commands:\n");
    cputs("  help                - Display command list\n");
    cputs("  clear / cls         - Clear screen\n");
    cputs("  echo <text>         - Print string to terminal\n");
    cputs("  about               - System & resolution specs\n");
    cputs("  paint / mspaint     - Launch MSPaint GUI program\n");
    cputs("  calc / calculator   - Launch Calculator GUI program\n");
  }
  // Command: about
  else if (strcmp(cmd_line, CMD_ABOUT) == 0) {
    const GraphicsState *gfx = get_graphics_state();
    char num_buf[16];

    cputs("OS Kernel Console v1.0\n");
    cputs("Resolution: ");
    itoa((int)gfx->width, num_buf);
    cputs(num_buf);
    cputs("x");
    itoa((int)gfx->height, num_buf);
    cputs(num_buf);
    cputs(" @ ");
    itoa((int)gfx->bits_per_pixel, num_buf);
    cputs(num_buf);
    cputs(" bpp\n");
  }
  // Command: paint / mspaint / draw -> Launch Paint GUI
  else if (strcmp(cmd_line, CMD_PAINT) == 0 ||
           strcmp(cmd_line, CMD_MSPAINT) == 0 ||
           strcmp(cmd_line, CMD_DRAW) == 0) {
    g_console_initialized = false;
    set_current_program((Program){PAINT_init, PAINT_run});
  }
  // Command: calc / calculator -> Launch Calculator GUI
  else if (strcmp(cmd_line, CMD_CALC) == 0 ||
           strcmp(cmd_line, CMD_CALCULATOR) == 0) {
    g_console_initialized = false;
    set_current_program((Program){CALC_GUI_init, CALC_GUI_run});
  } else {
    cputs("Unknown command: ");
    cputs(cmd_line);
    cputs("\nType 'help' for available commands.\n");
  }

  if (g_console_initialized && strcmp(cmd_line, CMD_CLEAR) != 0 &&
      strcmp(cmd_line, CMD_CLS) != 0) {
    cputs("\n");
  }
}

void CONSOLE_run(bool keyboard_changed, bool mouse_changed) {
  if (!g_console_initialized) {
    print_prompt();
    g_console_initialized = true;
  }

  if (keyboard_changed && kbhit()) {
    int8_t c = getch();

    if (c != KEY_NONE) {
      if (c == '\n' || c == '\r') {
        putch('\n');
        g_cmd_buffer[g_cmd_len] = '\0';
        execute_command(g_cmd_buffer);
        g_cmd_len = 0;

        // Re-render prompt if command didn't context-switch away to a GUI app
        if (g_console_initialized) {
          print_prompt();
        }
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

  if (mouse_changed || keyboard_changed) {
    render_console();
  }
}
