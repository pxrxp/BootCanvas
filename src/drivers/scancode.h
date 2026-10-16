#ifndef SCANCODE_H
#define SCANCODE_H

typedef enum {
  // Single-byte scan codes
  KEY_ESC = 0x01,
  KEY_1 = 0x02,
  KEY_2 = 0x03,
  KEY_3 = 0x04,
  KEY_4 = 0x05,
  KEY_5 = 0x06,
  KEY_6 = 0x07,
  KEY_7 = 0x08,
  KEY_8 = 0x09,
  KEY_9 = 0x0A,
  KEY_0 = 0x0B,
  KEY_MINUS = 0x0C, // -
  KEY_EQUAL = 0x0D, // =
  KEY_BACKSPACE = 0x0E,
  KEY_TAB = 0x0F,
  KEY_Q = 0x10,
  KEY_W = 0x11,
  KEY_E = 0x12,
  KEY_R = 0x13,
  KEY_T = 0x14,
  KEY_Y = 0x15,
  KEY_U = 0x16,
  KEY_I = 0x17,
  KEY_O = 0x18,
  KEY_P = 0x19,
  KEY_LEFT_BRACKET = 0x1A,  // [
  KEY_RIGHT_BRACKET = 0x1B, // ]
  KEY_ENTER = 0x1C,
  KEY_LEFT_CTRL = 0x1D,
  KEY_A = 0x1E,
  KEY_S = 0x1F,
  KEY_D = 0x20,
  KEY_F = 0x21,
  KEY_G = 0x22,
  KEY_H = 0x23,
  KEY_J = 0x24,
  KEY_K = 0x25,
  KEY_L = 0x26,
  KEY_SEMICOLON = 0x27,    // ;
  KEY_SINGLE_QUOTE = 0x28, // '
  KEY_GRAVE = 0x29,        // `
  KEY_LEFT_SHIFT = 0x2A,
  KEY_BACKSLASH = 0x2B, /* \ */
  KEY_Z = 0x2C,
  KEY_X = 0x2D,
  KEY_C = 0x2E,
  KEY_V = 0x2F,
  KEY_B = 0x30,
  KEY_N = 0x31,
  KEY_M = 0x32,
  KEY_COMMA = 0x33, // ,
  KEY_DOT = 0x34,   // .
  KEY_SLASH = 0x35, // /
  KEY_RIGHT_SHIFT = 0x36,
  KEY_KP_MULTIPLY = 0x37, // Keypad *
  KEY_LEFT_ALT = 0x38,
  KEY_SPACE = 0x39,
  KEY_CAPS_LOCK = 0x3A,
  KEY_F1 = 0x3B,
  KEY_F2 = 0x3C,
  KEY_F3 = 0x3D,
  KEY_F4 = 0x3E,
  KEY_F5 = 0x3F,
  KEY_F6 = 0x40,
  KEY_F7 = 0x41,
  KEY_F8 = 0x42,
  KEY_F9 = 0x43,
  KEY_F10 = 0x44,
  KEY_NUM_LOCK = 0x45,
  KEY_SCROLL_LOCK = 0x46,
  KEY_KP_7 = 0x47,
  KEY_KP_8 = 0x48,
  KEY_KP_9 = 0x49,
  KEY_KP_MINUS = 0x4A, // Keypad -
  KEY_KP_4 = 0x4B,
  KEY_KP_5 = 0x4C,
  KEY_KP_6 = 0x4D,
  KEY_KP_PLUS = 0x4E, // Keypad +
  KEY_KP_1 = 0x4F,
  KEY_KP_2 = 0x50,
  KEY_KP_3 = 0x51,
  KEY_KP_0 = 0x52,
  KEY_KP_DECIMAL = 0x53, // Keypad .
  KEY_F11 = 0x57,
  KEY_F12 = 0x58,

  KEY_INCOMPLETE = 0xE0,

  // Extended scan codes (0xE0 Prefix)
  KEY_KP_ENTER = 0xE01C, // Keypad EN
  KEY_RIGHT_CTRL = 0xE01D,
  KEY_KP_DIVIDE = 0xE035, // Keypad /
  KEY_RIGHT_ALT = 0xE038,
  KEY_HOME = 0xE047,
  KEY_UP_ARROW = 0xE048,
  KEY_PAGE_UP = 0xE049,
  KEY_LEFT_ARROW = 0xE04B,
  KEY_RIGHT_ARROW = 0xE04D,
  KEY_END = 0xE04F,
  KEY_DOWN_ARROW = 0xE050,
  KEY_PAGE_DOWN = 0xE051,
  KEY_INSERT = 0xE052,
  KEY_DELETE = 0xE053,
  KEY_LEFT_GUI = 0xE05B,
  KEY_RIGHT_GUI = 0xE05C,
  KEY_APPS = 0xE05D,

  // Multi-byte sequence scan codes
  // KEY_PAUSE = 0xE11D45,
  // KEY_PRNT_SCRN = 0xE02AE037
} ScanCode;

const char keycode_to_ascii[0x53 + 1] = {
    0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-',  '=',
    '\b', '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[',  ']',
    '\n', 0,    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,    '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,    '*',
    0,    ' ',  0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,    0,
    0,    '7',  '8', '9', '-', '4', '5', '6', '+', '1', '2', '3', '0',  '.'};

#endif
