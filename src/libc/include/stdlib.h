#ifndef STDLIB_H
#define STDLIB_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
  int quot;
  int rem;
} div_t;

int atoi(const char *str);
void itoa(int value, char *str);
void srand(unsigned int seed);
int rand();
int abs(int x);
div_t div(int numerator, int denominator);
void swap(uint8_t *a, uint8_t *b, size_t size);
void *bsearch(const void *key, const void *base, size_t num, size_t size,
              int (*compare)(const void *, const void *));
void qsort(void *base, size_t num, size_t size,
           int (*compare)(const void *, const void *));

#endif
