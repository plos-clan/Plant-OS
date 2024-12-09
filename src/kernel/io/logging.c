#include <kernel.h>

// 一些用于日志记录和打印的函数。

#define PORT 0x3f8 // 日志输出端口：COM1

#if LOG_EXT

//* ----------------------------------------------------------------------------------------------------
//; 扩展数据输出
//* ----------------------------------------------------------------------------------------------------

finline void log_out8(byte a) {
  waituntil(asm_in8(PORT + 5) & 0x20);
  asm_out8(PORT, a);
}

finline void log_out16(u16 a) {
  log_out8(a);
  log_out8(a >> 8);
}

finline void log_out32(u32 a) {
  log_out16(a);
  log_out16(a >> 16);
}

finline void log_out64(u64 a) {
  log_out32(a);
  log_out32(a >> 32);
}

finline void log_outp(const void *p) {
#  if __x86_64__
  log_out64((size_t)p);
#  else
  log_out32((size_t)p);
#  endif
}

finline void log_outb(const void *ptr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    log_out8(((const byte *)ptr)[i]);
  }
}

finline void log_outs(cstr s) {
  size_t n = 0;
  for (; s[n] != '\0'; n++) {}
  log_out32(n);
  for (size_t i = 0; i < n; i++) {
    log_out8(s[i]);
  }
}

#  define log_dump(data) log_outb(&(data), sizeof(data))

//* ----------------------------------------------------------------------------------------------------
//; 日志类型 ID
//* ----------------------------------------------------------------------------------------------------

enum {
  ID_MSG        = 0,
  ID_FUNC_ENTER = 1,
  ID_FUNC_EXIT  = 2,
};

//* ----------------------------------------------------------------------------------------------------
//; 函数调用追踪
//* ----------------------------------------------------------------------------------------------------

// __nif void __cyg_profile_func_enter(void *callee, void *caller) {
//   size_t flags = asm_get_flags();
//   asm_cli;
//   struct __PACKED__ {
//     u32   id;
//     void *callee;
//     void *caller;
//   } data = {ID_FUNC_ENTER, callee, caller};
//   log_dump(data);
//   asm_set_flags(flags);
// }

// __nif void __cyg_profile_func_exit(void *callee, void *caller) {
//   size_t flags = asm_get_flags();
//   asm_cli;
//   struct __PACKED__ {
//     u32   id;
//     void *callee;
//     void *caller;
//   } data = {ID_FUNC_EXIT, callee, caller};
//   log_dump(data);
//   asm_set_flags(flags);
// }

//* ----------------------------------------------------------------------------------------------------
//; 扩展日志记录函数
//* ----------------------------------------------------------------------------------------------------

void klog_raw(cstr s) {
  var flag = asm_get_flags();
  asm_cli;
  log_out32(ID_MSG);
  log_outs("Unknown");
  log_outs("Unknown");
  log_outs("Unknown");
  log_out32(-1);
  log_out32(cpuid_coreid);
  log_out32(-1);
  log_outs(s);
  asm_set_flags(flag);
}

void klog(cstr type, cstr file, cstr func, int line, cstr _rest fmt, ...) {
  var flag = asm_get_flags();
  asm_cli;
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  vsprintf(buf, fmt, va);
  va_end(va);
  log_outs(buf);
  asm_set_flags(flag);
}

static char print_buf[4096];

void kprintf(cstr _rest fmt1, cstr _rest fmt2, ...) {
  va_list va1, va2;
  va_start(va1, fmt2);
  va_copy(va2, va1);
  vsprintf(print_buf, fmt1, va1);
  klog_raw(print_buf);
  vsprintf(print_buf, fmt2, va2);
  print(print_buf);
  va_end(va2);
  va_end(va1);
}

int printf(cstr _rest fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(print_buf, fmt, va);
  va_end(va);
  print(print_buf);
  size_t len = strlen(print_buf);
  while (len > 0 && print_buf[len - 1] == '\n') {
    print_buf[len - 1] = '\0';
    len--;
  }
  klogi("print: %s", print_buf);
  return rets;
}

void puts(cstr s) {
  print(s);
  print("\n");
  klogi("print: %s", s);
}

#else

//* ----------------------------------------------------------------------------------------------------
//; 基本日志记录函数
//* ----------------------------------------------------------------------------------------------------

bool lastline_ended = true;

static char buf[1024];
static int  bufp = 0;

finline void _putb(int c) {
  waituntil(asm_in8(PORT + 5) & 0x20);
  asm_out8(PORT, c);
}

finline void _puts(cstr s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    _putb(s[i]);
  }
}

#  define _peek_cirbuf_len 64
static int  _peek_cache = -1;
static char _peek_cirbuf[_peek_cirbuf_len];
static int  _peek_cirbuf_h = 0;
static int  _peek_cirbuf_t = 0;

finline int _peekb_raw() {
  if (_peek_cache >= 0) return _peek_cache;
  if ((asm_in8(PORT + 5) & 1) == 0) return -1;
  _peek_cache = asm_in8(PORT);
  return _peek_cache;
}

finline int _getb_raw() {
  if (_peek_cache >= 0) {
    int c       = _peek_cache;
    _peek_cache = -1;
    return c;
  }
  if ((asm_in8(PORT + 5) & 1) == 0) return -1;
  return asm_in8(PORT);
}

finline int _peekb_block_raw() {
  if (_peek_cache >= 0) return _peek_cache;
  while ((asm_in8(PORT + 5) & 1) == 0) {}
  _peek_cache = asm_in8(PORT);
  return _peek_cache;
}

finline int _getb_block_raw() {
  if (_peek_cache >= 0) {
    int c       = _peek_cache;
    _peek_cache = -1;
    return c;
  }
  while ((asm_in8(PORT + 5) & 1) == 0) {}
  return asm_in8(PORT);
}

finline int _peekb() {
  if (_peek_cirbuf_h != _peek_cirbuf_t) return _peek_cirbuf[_peek_cirbuf_h];
  if (_peek_cache >= 0) return _peek_cache;
  if ((asm_in8(PORT + 5) & 1) == 0) return -1;
  _peek_cache = asm_in8(PORT);
  return _peek_cache;
}

finline int _getb() {
  if (_peek_cirbuf_h != _peek_cirbuf_t) {
    int c          = _peek_cirbuf[_peek_cirbuf_h];
    _peek_cirbuf_h = (_peek_cirbuf_h + 1) % _peek_cirbuf_len;
    return c;
  }
  if (_peek_cache >= 0) {
    int c       = _peek_cache;
    _peek_cache = -1;
    return c;
  }
  if ((asm_in8(PORT + 5) & 1) == 0) return -1;
  return asm_in8(PORT);
}

finline int _peekb_block() {
  if (_peek_cirbuf_h != _peek_cirbuf_t) return _peek_cirbuf[_peek_cirbuf_h];
  if (_peek_cache >= 0) return _peek_cache;
  while ((asm_in8(PORT + 5) & 1) == 0) {}
  _peek_cache = asm_in8(PORT);
  return _peek_cache;
}

finline int _getb_block() {
  if (_peek_cirbuf_h != _peek_cirbuf_t) {
    int c          = _peek_cirbuf[_peek_cirbuf_h];
    _peek_cirbuf_h = (_peek_cirbuf_h + 1) % _peek_cirbuf_len;
    return c;
  }
  if (_peek_cache >= 0) {
    int c       = _peek_cache;
    _peek_cache = -1;
    return c;
  }
  while ((asm_in8(PORT + 5) & 1) == 0) {}
  return asm_in8(PORT);
}

finline void _peek_skip() {
  assert(_peek_cache >= 0);
  _peek_cirbuf[_peek_cirbuf_t] = _peek_cache;
  _peek_cache                  = -1;
}

finline void _peek_skip_until(int c) {
  while (_peekb_block_raw() != c) {
    _peek_skip();
  }
}

finline int getnb(int *cp) {
  int num = 0;
  int c;
  while ((c = _getb()) >= 0) {
    if (c >= '0' && c <= '9') {
      num = num * 10 + (c - '0');
    } else {
      break;
    }
  }
  *cp = c;
  return num;
}

finline int getnb_block(int *cp) {
  int num = 0;
  int c;
  while ((c = _getb_block()) >= 0) {
    if (c >= '0' && c <= '9') {
      num = num * 10 + (c - '0');
    } else {
      break;
    }
  }
  *cp = c;
  return num;
}

finline int getnb_block_raw(int *cp) {
  int num = 0;
  int c;
  while ((c = _getb_block_raw()) >= 0) {
    if (c >= '0' && c <= '9') {
      num = num * 10 + (c - '0');
    } else {
      break;
    }
  }
  *cp = c;
  return num;
}

static int get_terminal_size(i32 *rows, i32 *cols) {
  _puts("\033[18t");
  _peek_skip_until('\033');
  if (_getb_block_raw() != '\033') return -1;
  if (_getb_block_raw() != '[') return -1;
  if (_getb_block_raw() != '8') return -1;
  if (_getb_block_raw() != ';') return -1;
  int c;
  *rows = getnb_block_raw(&c);
  if (c != ';') return -1;
  *cols = getnb_block_raw(&c);
  if (c != 't') return -1;
  return 0;
}

static int termx() {
  i32 rows = 0, cols = 0;
  get_terminal_size(&rows, &cols);
  return cols;
}

static int termy() {
  i32 rows = 0, cols = 0;
  get_terminal_size(&rows, &cols);
  return rows;
}

static int get_cursor_pos(i32 *row, i32 *col) {
  _puts("\033[6n");
  _peek_skip_until('\033');
  if (_getb_block_raw() != '\033') return -1;
  if (_getb_block_raw() != '[') return -1;
  int c;
  *row = getnb_block_raw(&c);
  if (c != ';') return -1;
  *col = getnb_block_raw(&c);
  if (c != 'R') return -1;
  return 0;
}

static int curx() {
  i32 row = 0, col = 0;
  get_cursor_pos(&row, &col);
  return col;
}

static int cury() {
  i32 row = 0, col = 0;
  get_cursor_pos(&row, &col);
  return row;
}

bool debug_enabled = false;

extern struct event debug_shell_event;
extern char         debug_shell_path[1024];

char debug_shell_message[256];

static int input_nlines = 1;

static void clear_input_view() {
  _puts(CEND);
  for (int i = 0; i < input_nlines; i++) {
    _puts("\033[2K\033[A");
  }
  _puts("\033[2K\033[999D");
}

static void print_input_buffer() {
  _puts(CRGB(153, 255, 231) CBRGB(95, 95, 240) "\033[2K at " CRGB(255, 192, 224));
  _puts(debug_shell_path);
  _puts(CRGB(153, 255, 231) "\033[999C");
  if (debug_shell_message[0] == '\0') strcpy(debug_shell_message, "Hello debug shell!");
  int n = strlen(debug_shell_message);
  for (int i = 0; i < n; i++) {
    _puts("\033[D");
  }
  for (int i = 0; i < n; i++) {
    const int   k = i * 256 / (n - 1);
    static char buffer[32];
    sprintf(buffer, "\033[38;2;%d;%d;%dm",      //
            (253 * k + 246 * (256 - k)) / 256,  //
            (133 * k + 211 * (256 - k)) / 256,  //
            (172 * k + 101 * (256 - k)) / 256); //
    _puts(buffer);
    _putb(debug_shell_message[i]);
  }
  _puts(CEND "\n");
  _puts(CRGB(255, 224, 183) "\033[2K" CRGB(183, 183, 255) ">" CEND " " CRGB(255, 224, 183));
  buf[bufp] = '\0';
  _puts(buf);
}

// 输出函数
finline void log_outs(cstr s) {
  if (s == null) return;
  clear_input_view();
  if (!lastline_ended) _puts("\033[A\033[999C");
  for (size_t i = 0; s[i] != '\0'; i++) {
    lastline_ended = s[i] == '\n';
    _putb(s[i]);
  }
  if (!lastline_ended) _putb('\n');
  print_input_buffer();
}

void log_update() {
  clear_input_view();
  print_input_buffer();
}

void klog_raw(cstr s) {
  var flag = asm_get_flags();
  asm_cli;
  log_outs(s);
  asm_set_flags(flag);
}

void klog(cstr _rest fmt, ...) {
  var flag = asm_get_flags();
  asm_cli;
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  vsprintf(buf, fmt, va);
  va_end(va);
  log_outs(buf);
  asm_set_flags(flag);
}

static char print_buf[4096];

void kprintf(cstr _rest fmt1, cstr _rest fmt2, ...) {
  va_list va1, va2;
  va_start(va1, fmt2);
  va_copy(va2, va1);
  vsprintf(print_buf, fmt1, va1);
  klog_raw(print_buf);
  vsprintf(print_buf, fmt2, va2);
  print(print_buf);
  va_end(va2);
  va_end(va1);
}

int printf(cstr _rest fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(print_buf, fmt, va);
  va_end(va);
  print(print_buf);
  size_t len = strlen(print_buf);
  while (len > 0 && print_buf[len - 1] == '\n') {
    print_buf[len - 1] = '\0';
    len--;
  }
  klogi("print: %s", print_buf);
  return rets;
}

void puts(cstr s) {
  print(s);
  print("\n");
  klogi("print: %s", s);
}

void debugger() {
  if (!debug_enabled) return;
  int c = 0;
  while (true) {
    while ((c = _getb()) >= 0) {
      if (c == '\n' || c == '\r') break;
      if (c == 127) {
        if (bufp > 0) {
          _puts("\b \b");
          bufp--;
          buf[bufp] = '\0';
          if (curx() == 2) {
            input_nlines--;
            _puts("\033[T\033[999C ");
          }
        }
      } else {
        if (c == '\e') {
          if (_getb_block() == '[') {
            switch (_getb_block()) {
            case 'A': break;
            case 'B': break;
            case 'C': break;
            case 'D': break;
            }
          }
        } else {
          _putb(c);
          buf[bufp] = c;
          bufp++;
          if (curx() == 2) {
            input_nlines++;
            _puts("\033[2K\033[999D" CRGB(183, 183, 255) "|" CEND " " CRGB(255, 224, 183));
            _putb(c);
          }
        }
      }
    }
    if (c < 0) break;
    buf[bufp] = '\0';
    event_push(&debug_shell_event, strdup(buf));
    bufp = 0;
    _puts("\033[2K\033[999D" CRGB(183, 183, 255) ">" CEND " " CRGB(255, 224, 183));
  }
}

#endif
