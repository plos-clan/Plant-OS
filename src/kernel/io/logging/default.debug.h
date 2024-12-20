#include <kernel.h>

#ifndef PORT
#  warning "PORT is not defined"
#  define PORT 0
#endif

//* ----------------------------------------------------------------------------------------------------
//; 基本日志记录函数
//* ----------------------------------------------------------------------------------------------------

static bool lastline_ended = true;

static char debug_cmd_line[1024];
static int  debug_cmd_line_p = 0;

finline void _putb(int c) {
  waituntil(asm_in8(PORT + 5) & 0x20);
  asm_out8(PORT, c);
}

finline void _puts(cstr s) {
  for (size_t i = 0; s[i] != '\0'; i++) {
    _putb(s[i]);
  }
}

#define _peek_cirbuf_len 64
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
  waitif((asm_in8(PORT + 5) & 1) == 0);
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
  waitif((asm_in8(PORT + 5) & 1) == 0);
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
  debug_cmd_line[debug_cmd_line_p] = '\0';
  _puts(debug_cmd_line);
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
  with_no_interrupts({
    clear_input_view();
    print_input_buffer();
  });
}

void klog_raw(cstr s) {
  with_no_interrupts(log_outs(s));
}

static char klog_buf[4096];

void klog(cstr _rest fmt, ...) {
  with_no_interrupts({
    va_list va;
    va_start(va, fmt);
    vsprintf(klog_buf, fmt, va);
    va_end(va);
    log_outs(klog_buf);
  });
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

static void _debugger_tick() {
  int c = 0;
  while (true) {
    while ((c = _getb()) >= 0) {
      if (c == '\n' || c == '\r') break;
      if (c == 127) {
        if (debug_cmd_line_p > 0) {
          _puts("\b \b");
          debug_cmd_line_p--;
          debug_cmd_line[debug_cmd_line_p] = '\0';
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
          debug_cmd_line[debug_cmd_line_p] = c;
          debug_cmd_line_p++;
          if (curx() == 2) {
            input_nlines++;
            _puts("\033[2K\033[999D" CRGB(183, 183, 255) "|" CEND " " CRGB(255, 224, 183));
            _putb(c);
          }
        }
      }
    }
    if (c < 0) break;
    debug_cmd_line[debug_cmd_line_p] = '\0';
    event_push(&debug_shell_event, strdup(debug_cmd_line));
    debug_cmd_line_p = 0;
    _puts("\033[2K\033[999D" CRGB(183, 183, 255) ">" CEND " " CRGB(255, 224, 183));
  }
}

void kenel_debugger_tick() {
  if (!debug_enabled) return;
  with_no_interrupts(_debugger_tick());
}
