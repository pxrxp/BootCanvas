#include <stdlib.h>
#include <stdint.h>

static int Xn;

int a = 0;
int c = 0;
int m = 0;

void srand(unsigned int seed) { Xn = seed; }

int rand() {
  Xn = (a * Xn + c) % m;
  return Xn;
}

int atoi(const char *str) {
  const char *c = str;
  int8_t sign;
  int number = 0;

  while (*c) {
    if (*c == '-') {
      sign = -1;
    }

    if (*c >= '0' && *c <= '9') {
      number *= 10;
      number += *c - '0';
    }

    c++;
  }

  return number * sign;
}

double atof(const char *str) {
  const char *c = str;
  int8_t sign;
  double number = 0;

  while (*c) {
    if (*c == '-') {
      sign = -1;
    }

    if (*c >= '0' && *c <= '9') {
      number *= 10;
      number += *c - '0';
    }

    c++;
  }

  return number * sign;
}

void itoa(int value, char *str) {
  char *c = str;
  int num = value;

  if (num == 0) {
    *c++ = '0';
  }

  if (num < 0) {
    num *= -1;
    *c++ = '-';
  }

  int reversed = 0;
  while (num > 0) {
    reversed *= 10;
    reversed += (num % 10);
    num /= 10;
  }

  while (reversed > 0) {
    *c++ = '0' + (reversed % 10);
    reversed /= 10;
  }

  *c = '\0';
}
