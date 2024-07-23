#include <dosldr.h>
#define LONG_MAX 0x7fffffff
#define LONG_MIN (-LONG_MAX - 1)
// strcmp
void printk(char *s, ...) {}

#define FLAG_ALTNT_FORM    0x01
#define FLAG_ALTNT_FORM_CH '#'

#define FLAG_ZERO_PAD    0x02
#define FLAG_ZERO_PAD_CH '0'

#define FLAG_LEFT_ADJUST    0x04
#define FLAG_LEFT_ADJUST_CH '-'

#define FLAG_SPACE_BEFORE_POS_NUM    0x08
#define FLAG_SPACE_BEFORE_POS_NUM_CH ' '

#define FLAG_SIGN    0x10
#define FLAG_SIGN_CH '+'

#define FLAG_LOWER 0x20

#define INT_TYPE_CHAR      0x1
#define INT_TYPE_SHORT     0x2
#define INT_TYPE_INT       0x4
#define INT_TYPE_LONG      0x8
#define INT_TYPE_LONG_LONG 0x10
#define INT_TYPE_MIN       INT_TYPE_CHAR
#define INT_TYPE_MAX       INT_TYPE_LONG_LONG

#define BUF_SIZE 4096

static char str_buf[BUF_SIZE];
static char num_str_buf[BUF_SIZE];

static char *int32_to_str_dec(int32_t num, int flag, int width) {
  int32_t num_tmp   = num;
  char   *p         = num_str_buf;
  char   *q         = NULL;
  int     len       = 0;
  char   *str_first = NULL;
  char   *str_end   = NULL;
  char    ch        = 0;

  memset(num_str_buf, 0, sizeof(num_str_buf));

  char dic[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

  if (num_tmp < 0) {
    *p++ = '-';
  } else if (flag & FLAG_SIGN) {
    *p++ = '+';
  } else {
    *p++ = ' ';
  }
  str_first = num_str_buf;

  if (num_tmp >= 0 && !(flag & FLAG_SIGN)) { str_first++; }

  if (num_tmp == 0) {
    *p++ = '0';
  } else {
    if (num_tmp < 0) num_tmp = -num_tmp;

    while (num_tmp) {
      *p++     = dic[num_tmp % 10];
      num_tmp /= 10;
    }
  }
  *p = '\0';

  str_end = p;
  len     = str_end - str_first;

  q = num_str_buf + 1;
  p--;
  while (q < p) {
    ch = *q;
    *q = *p;
    *p = ch;
    p--;
    q++;
  }

  if (len < width) {
    p = str_end;

    if (flag & FLAG_LEFT_ADJUST) {
      for (int i = 0; i < width - len; i++)
        *p++ = ' ';
      *p      = '\0';
      str_end = p;
    } else {
      while (p >= str_first) {
        *(p + width - len) = *p;
        p--;
      }

      if (flag & FLAG_ZERO_PAD) {
        for (int i = 0; i < width - len; i++) {
          num_str_buf[i + 1] = '0';
        }
      } else {
        for (int i = 0; i < width - len; i++)
          str_first[i] = ' ';
      }
    }
  }

  return str_first;
}

static char *int64_to_str_dec(int64_t num, int flag, int width) {
  return 0;
}

static char *uint32_to_str_hex(u32 num, int flag, int width) {
  u32   num_tmp   = num;
  char *p         = num_str_buf;
  char *q         = NULL;
  int   len       = 0;
  char *str_first = NULL;
  char *str_end   = NULL;
  char  ch        = 0;

  memset(num_str_buf, 0, sizeof(num_str_buf));

  char  dic_lower[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  char  dic_upper[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char *dic         = (flag & FLAG_LOWER) ? dic_lower : dic_upper;

  str_first = num_str_buf;

  *p++ = '0';
  *p++ = (flag & FLAG_LOWER) ? 'x' : 'X';

  if (!(flag & FLAG_ALTNT_FORM)) { str_first += 2; }

  do {
    *p++     = dic[num_tmp % 16];
    num_tmp /= 16;
  } while (num_tmp);
  *p = '\0';

  str_end = p;
  len     = str_end - str_first;

  q = num_str_buf + 2;
  p--;
  while (q < p) {
    ch = *q;
    *q = *p;
    *p = ch;
    p--;
    q++;
  }

  if (len < width) {
    p = str_end;

    if (flag & FLAG_LEFT_ADJUST) {
      for (int i = 0; i < width - len; i++)
        *p++ = ' ';
      *p      = '\0';
      str_end = p;
    } else {
      while (p >= str_first) {
        *(p + width - len) = *p;
        p--;
      }
      if (flag & FLAG_ALTNT_FORM) str_first[1] = (flag & FLAG_LOWER) ? 'x' : 'X';

      if (flag & FLAG_ZERO_PAD) {
        for (int i = 0; i < width - len; i++) {
          num_str_buf[i + 2] = '0';
        }
      } else {
        for (int i = 0; i < width - len; i++)
          str_first[i] = ' ';
      }
    }
  }

  if (num == 0 && flag & FLAG_ALTNT_FORM) str_first[1] = '0';

  return str_first;
}

static char *uint64_to_str_hex(u64 num, int flag, int width) {
  u64   num_tmp   = num;
  char *p         = num_str_buf;
  char *q         = NULL;
  int   len       = 0;
  char *str_first = NULL;
  char *str_end   = NULL;
  char  ch        = 0;

  memset(num_str_buf, 0, sizeof(num_str_buf));

  char  dic_lower[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
  char  dic_upper[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                       '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};
  char *dic         = (flag & FLAG_LOWER) ? dic_lower : dic_upper;

  str_first = num_str_buf;

  *p++ = '0';
  *p++ = (flag & FLAG_LOWER) ? 'x' : 'X';

  if (!(flag & FLAG_ALTNT_FORM)) { str_first += 2; }

  while (num_tmp) {
    *p++     = dic[num_tmp % 16];
    num_tmp /= 16;
  }
  *p = '\0';

  str_end = p;
  len     = str_end - str_first;

  q = num_str_buf + 2;
  p--;
  while (q < p) {
    ch = *q;
    *q = *p;
    *p = ch;
    p--;
    q++;
  }

  if (len < width) {
    p = str_end;

    while (p >= str_first) {
      *(p + width - len) = *p;
      p--;
    }

    if (flag & FLAG_ZERO_PAD) {
      for (int i = 0; i < width - len; i++) {
        num_str_buf[i + 2] = '0';
      }
    } else {
      for (int i = 0; i < width - len; i++)
        str_first[i] = ' ';
    }
  }

  return str_first;
}

static char *uint32_to_str_oct(u32 num, int flag, int width) {
  u32   num_tmp   = num;
  char *p         = num_str_buf;
  char *q         = NULL;
  int   len       = 0;
  char *str_first = NULL;
  char *str_end   = NULL;
  char  ch        = 0;

  memset(num_str_buf, 0, sizeof(num_str_buf));

  char dic[] = {'0', '1', '2', '3', '4', '5', '6', '7'};

  str_first = num_str_buf;

  *p++ = '0';

  if (!(flag & FLAG_ALTNT_FORM)) { str_first += 1; }

  while (num_tmp) {
    *p++     = dic[num_tmp % 8];
    num_tmp /= 8;
  }
  *p = '\0';

  str_end = p;
  len     = str_end - str_first;

  q = num_str_buf + 1;
  p--;
  while (q < p) {
    ch = *q;
    *q = *p;
    *p = ch;
    p--;
    q++;
  }

  if (len < width) {
    p = str_end;

    if (flag & FLAG_LEFT_ADJUST) {
      for (int i = 0; i < width - len; i++)
        *p++ = ' ';
      *p      = '\0';
      str_end = p;
    } else {
      while (p >= str_first) {
        *(p + width - len) = *p;
        p--;
      }

      if (flag & FLAG_ZERO_PAD) {
        for (int i = 0; i < width - len; i++) {
          num_str_buf[i + 1] = '0';
        }
      } else {
        for (int i = 0; i < width - len; i++)
          str_first[i] = ' ';
      }
    }
  }

  return str_first;
}

static char *insert_str(char *buf, const char *str) {
  char *p = buf;

  while (*str) {
    *p++ = *str++;
  }

  return p;
}

void strrev(char *s) {
  if (NULL == s) return;

  char *pBegin = s;
  char *pEnd   = s + strlen(s) - 1;

  char pTemp;

  while (pBegin < pEnd) {
    pTemp   = *pBegin;
    *pBegin = *pEnd;
    *pEnd   = pTemp;

    ++pBegin, --pEnd;
  }
}

int printf(cstr fmt, ...) {
  int     len;
  va_list ap;
  va_start(ap, fmt);
  char buf[1024];
  len = vsprintf(buf, fmt, ap);
  print(buf);
  va_end(ap);
  return len;
}

int logf(const char *format, ...) {
  int     len;
  va_list ap;
  va_start(ap, format);
  char buf[1024];
  len = vsprintf(buf, format, ap);
  print(buf);
  va_end(ap);
  return len;
}
int _Znaj(u32 size) {
  printk("_Znaj:%d\n", size);
  return (int)malloc(size);
}
void _ZdaPv(void *ptr) {
  printk("_ZdaPv:%08x\n", ptr);
  free(ptr);
}
//_ZdlPvj
void _ZdlPvj(void *ptr, u32 size) {
  printk("_ZdlPvj %08x %d\n", ptr, size);
  free(ptr);
}
//_Znwj
void *_Znwj(u32 size) {
  printk("_Znwj:%d\n", size);
  return malloc(size);
}

int snprintf(char *str, size_t size, const char *format, ...) {
  va_list args;
  va_start(args, format);
  int len = vsprintf(str, format, args);
  va_end(args);
  return len;
}
void assert(int expression) {
  if (!expression) { printk("AN ERROR\n"); }
}

void *calloc(u32 num, u32 size) {
  return malloc(num * size);
}
int sort1(const void *e1, const void *e2) {
  return *(int *)e1 - *(int *)e2;
}
int sort2(const void *e1, const void *e2) {
  return *(char *)e1 - *(char *)e2;
}

void qsort(void *base, size_t num, size_t width, int (*sort)(const void *e1, const void *e2)) {
  for (int i = 0; i < (int)num - 1; i++) {
    for (int j = 0; j < (int)num - 1 - i; j++) {
      if (sort((char *)base + j * width, (char *)base + (j + 1) * width) > 0) {
        for (int i = 0; i < (int)width; i++) {
          char temp                           = ((char *)base + j * width)[i];
          ((char *)base + j * width)[i]       = ((char *)base + (j + 1) * width)[i];
          ((char *)base + (j + 1) * width)[i] = temp;
        }
      }
    }
  }
}