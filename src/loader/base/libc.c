#include <dosldr.h>
#define LONG_MAX 0x7fffffff
#define LONG_MIN (-LONG_MAX - 1)
// strcmp
void printk(char *s, ...) {}

// strtol
long strtol(const char *nptr, char **endptr, int base) {
  long acc = 0;
  int  c;
  u32  cutoff;
  int  neg = 0, any, cutlim;

  /*
   * Skip white space and pick up leading +/- sign if any.
   * If base is 0, allow 0x for hex and 0 for octal, else
   * assume decimal; if base is already 16, allow 0x.
   */
  do {
    c = *nptr++;
  } while (isspace(c));
  if (c == '-') {
    neg = 1;
    c   = *nptr++;
  } else if (c == '+')
    c = *nptr++;
  if ((base == 0 || base == 16) && c == '0' && (*nptr == 'x' || *nptr == 'X')) {
    c     = nptr[1];
    nptr += 2;
    base  = 16;
  } else if ((base == 0 || base == 2) && c == '0' && (*nptr == 'b' || *nptr == 'B')) {
    c     = nptr[1];
    nptr += 2;
    base  = 2;
  } else if (base == 0)
    base = c == '0' ? 8 : 10;

  /*
   * Compute the cutoff value between legal numbers and illegal
   * numbers.  That is the largest legal value, divided by the
   * base.  An input number that is greater than this value, if
   * followed by a legal input character, is too big.  One that
   * is equal to this value may be valid or not; the limit
   * between valid and invalid numbers is then based on the last
   * digit.  For instance, if the range for longs is
   * [-2147483648..2147483647] and the input base is 10,
   * cutoff will be set to 214748364 and cutlim to either
   * 7 (neg==0) or 8 (neg==1), meaning that if we have accumulated
   * a value > 214748364, or equal but the next digit is > 7 (or 8),
   * the number is too big, and we will return a range error.
   */
  cutoff  = neg ? -(u32)LONG_MIN : LONG_MAX;
  cutlim  = cutoff % (u32)base;
  cutoff /= (u32)base;
  for (acc = 0, any = 0;; c = *nptr++) {
    if (isdigit(c))
      c -= '0';
    else if (isalpha(c))
      c -= isupper(c) ? 'A' - 10 : 'a' - 10;
    else
      break;
    if (c >= base) break;
    if (any < 0 || acc > cutoff || (acc == cutoff && c > cutlim))
      any = -1;
    else {
      any  = 1;
      acc *= base;
      acc += c;
    }
  }
  if (any < 0) {
    acc = neg ? LONG_MIN : LONG_MAX;
    // errno = ERANGE;
    print("panic: strtol: overflow\n");
  } else if (neg)
    acc = -acc;
  if (endptr != 0) *endptr = (char *)(any ? nptr : (char *)nptr - 1);
  return (acc);
}

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

int vsprintf(char *buf, const char *fmt, va_list args) {
  char   *str       = buf;
  int     flag      = 0;
  int     int_type  = INT_TYPE_INT;
  int     tot_width = 0;
  int     sub_width = 0;
  char    buf2[64]  = {0};
  char   *s         = NULL;
  char    ch        = 0;
  int8_t  num_8     = 0;
  u8      num_u8    = 0;
  int16_t num_16    = 0;
  u16     num_u16   = 0;
  int32_t num_32    = 0;
  u32     num_u32   = 0;
  int64_t num_64    = 0;
  u64     num_u64   = 0;

  for (const char *p = fmt; *p; p++) {
    if (*p != '%') {
      *str++ = *p;
      continue;
    }

    flag      = 0;
    tot_width = 0;
    sub_width = 0;
    int_type  = INT_TYPE_INT;

    p++;

    while (*p == FLAG_ALTNT_FORM_CH || *p == FLAG_ZERO_PAD_CH || *p == FLAG_LEFT_ADJUST_CH ||
           *p == FLAG_SPACE_BEFORE_POS_NUM_CH || *p == FLAG_SIGN_CH) {
      if (*p == FLAG_ALTNT_FORM_CH) {
        flag |= FLAG_ALTNT_FORM;
      } else if (*p == FLAG_ZERO_PAD_CH) {
        flag |= FLAG_ZERO_PAD;
      } else if (*p == FLAG_LEFT_ADJUST_CH) {
        flag |= FLAG_LEFT_ADJUST;
        flag &= ~FLAG_ZERO_PAD;
      } else if (*p == FLAG_SPACE_BEFORE_POS_NUM_CH) {
        flag |= FLAG_SPACE_BEFORE_POS_NUM;
      } else if (*p == FLAG_SIGN_CH) {
        flag |= FLAG_SIGN;
      } else {
      }

      p++;
    }

    if (*p == '*') {
      tot_width = va_arg(args, int);
      if (tot_width < 0) tot_width = 0;
      p++;
    } else {
      while (isdigit(*p)) {
        tot_width = tot_width * 10 + *p - '0';
        p++;
      }
    }
    if (*p == '.') {
      if (*p == '*') {
        sub_width = va_arg(args, int);
        if (sub_width < 0) sub_width = 0;
        p++;
      } else {
        while (isdigit(*p)) {
          sub_width = sub_width * 10 + *p - '0';
          p++;
        }
      }
    }

  LOOP_switch:
    switch (*p) {
    case 'h':
      p++;
      if (int_type >= INT_TYPE_MIN) {
        int_type >>= 1;
        goto LOOP_switch;
      } else {
        *str++ = '%';
        break;
      }
    case 'l':
      p++;
      if (int_type <= INT_TYPE_MAX) {
        int_type <<= 1;
        goto LOOP_switch;
      } else {
        *str++ = '%';
        break;
      }
    case 's':
      s   = va_arg(args, char *);
      str = insert_str(str, s);
      break;
    case 'c':
      ch     = (char)(va_arg(args, int) & 0xFF);
      *str++ = ch;
      break;
    case 'd':
      switch (int_type) {
      case INT_TYPE_CHAR:
        num_8 = (int8_t)va_arg(args, int32_t);
        str   = insert_str(str, int32_to_str_dec(num_8, flag, tot_width));
        break;
      case INT_TYPE_SHORT:
        num_16 = (int16_t)va_arg(args, int32_t);
        str    = insert_str(str, int32_to_str_dec(num_16, flag, tot_width));
        break;
      case INT_TYPE_INT:
        num_32 = va_arg(args, int32_t);
        str    = insert_str(str, int32_to_str_dec(num_32, flag, tot_width));
        break;
      case INT_TYPE_LONG:
        num_64 = va_arg(args, int64_t);
        str    = insert_str(str, int64_to_str_dec(num_64, flag, tot_width));
        break;
      case INT_TYPE_LONG_LONG:
        num_64 = va_arg(args, int64_t);
        str    = insert_str(str, int64_to_str_dec(num_64, flag, tot_width));
        break;
      }
      break;
    case 'x': flag |= FLAG_LOWER;
    case 'X':
      switch (int_type) {
      case INT_TYPE_CHAR:
        num_u8 = (u8)va_arg(args, u32);
        str    = insert_str(str, uint32_to_str_hex(num_u8, flag, tot_width));
        break;
      case INT_TYPE_SHORT:
        num_u16 = (u16)va_arg(args, u32);
        str     = insert_str(str, uint32_to_str_hex(num_u16, flag, tot_width));
        break;
      case INT_TYPE_INT:
        num_u32 = va_arg(args, u32);
        str     = insert_str(str, uint32_to_str_hex(num_u32, flag, tot_width));
        break;
      case INT_TYPE_LONG:
        num_u64 = va_arg(args, u64);
        str     = insert_str(str, uint64_to_str_hex(num_u64, flag, tot_width));
        break;
      case INT_TYPE_LONG_LONG:
        num_u64 = va_arg(args, u64);
        str     = insert_str(str, uint64_to_str_hex(num_u64, flag, tot_width));
        break;
      }
      break;
    case 'o':
      num_u32 = va_arg(args, u32);
      str     = insert_str(str, uint32_to_str_oct(num_u32, flag, tot_width));
      break;
    case '%': *str++ = '%'; break;
    default:
      *str++ = '%';
      *str++ = *p;
      break;
    }
  }
  *str = '\0';

  return str - buf;
}
// sprintf
int sprintf(char *buf, const char *fmt, ...) {
  va_list args;
  va_start(args, fmt);
  int len = vsprintf(buf, fmt, args);
  va_end(args);
  return len;
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
int printf(const char *format, ...) {
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

int vsnprintf(char *str, u32 size, const char *format, va_list ap) {
  return vsprintf(str, format, ap);
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