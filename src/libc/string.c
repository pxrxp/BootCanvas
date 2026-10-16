#include <stddef.h>
#include <string.h>

size_t strlen(const char *str) {
  size_t len = 0;
  while (str[len]) {
    len++;
  }
  return len;
}

int strcmp(const char *lhs, const char *rhs) {
  const unsigned char *left = (const unsigned char *)lhs;
  const unsigned char *right = (const unsigned char *)rhs;

  while (*left && *left == *right) {
    left++;
    right++;
  }

  return *left - *right;
}

int strncmp(const char *lhs, const char *rhs, size_t count) {
  const unsigned char *left = (const unsigned char *)lhs;
  const unsigned char *right = (const unsigned char *)rhs;

  for (size_t i = 0; i < count; i++) {
    if (left[i] != right[i] || left[i] == '\0') {
      return left[i] - right[i];
    }
  }

  return 0;
}

int memcmp(const void *lhs, const void *rhs, size_t count) {
  const unsigned char *left = (const unsigned char *)lhs;
  const unsigned char *right = (const unsigned char *)rhs;

  for (size_t i = 0; i < count; i++) {
    if (left[i] != right[i]) {
      return left[i] - right[i];
    }
  }

  return 0;
}

char *strcpy(char *dest, const char *src) {
  char *d = dest;
  while ((*d++ = *src++))
    ;
  return dest;
}

char *strncpy(char *dest, const char *src, size_t count) {
  size_t i;

  for (i = 0; i < count && src[i] != '\0'; i++) {
    dest[i] = src[i];
  }
  for (; i < count; i++) {
    dest[i] = '\0';
  }

  return dest;
}

void *memcpy(void *dest, const void *src, size_t count) {
  unsigned char *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;

  while (count--) {
    *d++ = *s++;
  }

  return dest;
}

void *memmove(void *dest, const void *src, size_t count) {
  unsigned char *d = (unsigned char *)dest;
  const unsigned char *s = (const unsigned char *)src;

  if (d < s) {
    while (count--) {
      *d++ = *s++;
    }
  } else if (d > s) {
    d += count;
    s += count;
    while (count--) {
      *--d = *--s;
    }
  }

  return dest;
}

char *strcat(char *dest, const char *src) {
  char *d = dest;

  while (*d) {
    d++;
  }
  while ((*d++ = *src++))
    ;

  return dest;
}

char *strncat(char *dest, const char *src, size_t count) {
  char *d = dest;

  while (*d) {
    d++;
  }
  while (count-- && *src) {
    *d++ = *src++;
  }
  *d = '\0';

  return dest;
}

void *memset(void *dest, int ch, size_t count) {
  unsigned char *d = (unsigned char *)dest;
  unsigned char c = (unsigned char)ch;

  while (count--) {
    *d++ = c;
  }

  return dest;
}

char *strchr(const char *str, int ch) {
  char c = (char)ch;

  while (*str != c) {
    if (*str == '\0') {
      return NULL;
    }
    str++;
  }

  return (char *)str;
}

char *strrchr(const char *str, int ch) {
  char c = (char)ch;
  const char *last = NULL;

  do {
    if (*str == c) {
      last = str;
    }
  } while (*str++);

  return (char *)last;
}

size_t strspn(const char *dest, const char *src) {
  size_t count = 0;

  while (*dest) {
    const char *s = src;
    while (*s && *s != *dest) {
      s++;
    }
    if (*s == '\0') {
      break;
    }
    count++;
    dest++;
  }

  return count;
}

size_t strcspn(const char *dest, const char *src) {
  size_t count = 0;

  while (*dest) {
    const char *s = src;
    while (*s) {
      if (*s == *dest) {
        return count;
      }
      s++;
    }
    count++;
    dest++;
  }

  return count;
}

char *strpbrk(const char *dest, const char *breakset) {
  while (*dest) {
    const char *b = breakset;
    while (*b) {
      if (*b == *dest) {
        return (char *)dest;
      }
      b++;
    }
    dest++;
  }

  return NULL;
}

char *strstr(const char *str, const char *substr) {
  if (!*substr) {
    return (char *)str;
  }

  while (*str) {
    const char *h = str;
    const char *n = substr;

    while (*h && *n && *h == *n) {
      h++;
      n++;
    }

    if (!*n) {
      return (char *)str;
    }

    str++;
  }

  return NULL;
}

void *memchr(const void *ptr, int ch, size_t count) {
  const unsigned char *p = (const unsigned char *)ptr;
  unsigned char c = (unsigned char)ch;

  while (count--) {
    if (*p == c) {
      return (void *)p;
    }
    p++;
  }

  return NULL;
}

char *strtok(char *str, const char *delim) {
  static char *saved_str = NULL;

  if (str != NULL) {
    saved_str = str;
  }
  if (saved_str == NULL) {
    return NULL;
  }

  while (*saved_str) {
    const char *d = delim;
    int is_delim = 0;
    while (*d) {
      if (*saved_str == *d) {
        is_delim = 1;
        break;
      }
      d++;
    }
    if (!is_delim) {
      break;
    }
    saved_str++;
  }

  if (*saved_str == '\0') {
    saved_str = NULL;
    return NULL;
  }

  char *token_start = saved_str;

  while (*saved_str) {
    const char *d = delim;
    while (*d) {
      if (*saved_str == *d) {
        *saved_str = '\0';
        saved_str++;
        return token_start;
      }
      d++;
    }
    saved_str++;
  }

  saved_str = NULL;
  return token_start;
}
