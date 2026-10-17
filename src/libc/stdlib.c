#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

static uint32_t state = 123456789;

void srand(unsigned int seed) { state = seed; }

int rand(void) {
  state = 1664525U * state + 1013904223U;
  return (int)((state >> 1) & 0x7FFFFFFF);
}

div_t div(int numerator, int denominator) {
  return (div_t){numerator / denominator, numerator % denominator};
}

void *bsearch(const void *key, const void *base, size_t num, size_t size,
              int (*compare)(const void *, const void *)) {
  const uint8_t *p = (const uint8_t *)base;
  size_t low = 0;
  size_t high = num;

  while (low < high) {
    size_t mid = low + (high - low) / 2;
    const uint8_t *elem = p + mid * size;
    int res = compare(key, elem);

    if (res < 0) {
      high = mid;
    } else if (res > 0) {
      low = mid + 1;
    } else {
      return (void *)elem;
    }
  }

  return NULL;
}

void swap(uint8_t *a, uint8_t *b, size_t size) {
  while (size--) {
    uint8_t temp = *a;
    *a++ = *b;
    *b++ = temp;
  }
}

void qsort(void *base, size_t num, size_t size,
           int (*compare)(const void *, const void *)) {
  if (num < 2 || size == 0) {
    return;
  }

  uint8_t *b = (uint8_t *)base;
  size_t pivot_idx = num / 2;

  swap(b + pivot_idx * size, b + (num - 1) * size, size);

  size_t store_idx = 0;
  for (size_t i = 0; i < num - 1; i++) {
    if (compare(b + i * size, b + (num - 1) * size) < 0) {
      if (i != store_idx) {
        swap(b + i * size, b + store_idx * size, size);
      }
      store_idx++;
    }
  }

  swap(b + store_idx * size, b + (num - 1) * size, size);

  qsort(b, store_idx, size, compare);
  qsort(b + (store_idx + 1) * size, num - store_idx - 1, size, compare);
}

int atoi(const char *str) {
  const char *c = str;
  int8_t sign = 1;
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
  int8_t sign = 1;
  double number = 0;

  double divisor = 1.0;
  bool decimal_point_seen = false;

  while (*c) {
    if (*c == '-') {
      sign = -1;
    }

    if (*c == '.') {
      decimal_point_seen = true;
    }

    if (*c >= '0' && *c <= '9') {
      number *= 10;
      number += *c - '0';

      if (decimal_point_seen) {
        divisor *= 10;
      }
    }

    c++;
  }

  return number * sign / divisor;
}

void itoa(int value, char *str) {
  char *c = str;
  unsigned int num;

  if (value == 0) {
    *c++ = '0';
    *c = '\0';
    return;
  }

  if (value < 0) {
    *c++ = '-';
    num = -(unsigned int)value;
  } else {
    num = (unsigned int)value;
  }

  char *start = c;
  while (num > 0) {
    *c++ = '0' + (num % 10);
    num /= 10;
  }
  *c = '\0';

  c--;
  while (start < c) {
    char temp = *start;
    *start++ = *c;
    *c-- = temp;
  }
}

int abs(int x) { return x >= 0 ? x : -x; }
int max(int x, int y) { return x >= y ? x : y; }
int min(int x, int y) { return x < y ? x : y; }
int clamp(int x, int low, int high) { return max(min(x, high), low); }
