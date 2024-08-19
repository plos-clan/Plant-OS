#pragma once
#include <libc.h>
#include <data-structure.h>

#define PL_READLINE_KEY_UP 0xff00
#define PL_READLINE_KEY_DOWN 0xff01
#define PL_READLINE_KEY_LEFT 0xff02
#define PL_READLINE_KEY_RIGHT 0xff03
#define PL_READLINE_KEY_ENTER '\n'
#define PL_READLINE_KEY_TAB '\t'
#define PL_READLINE_KEY_BACKSPACE '\b'
#define _THIS pl_readline_t self
#define PL_READLINE_SUCCESS 0
#define PL_READLINE_FAILED -1

typedef struct pl_readline_word {
  char *word;
  /**
    如果first为true，
    这个word必须在第一个参数的位置的时候才能得到补全
    如abc 则必须输入"ab"然后按tab，才会有可能有"abc"
    如果是“qwe ab”则不会补全"qwe abc"，除非first为false.
  */
  bool first;
} pl_readline_word;

typedef struct pl_readline_words {
  int len;
  pl_readline_word *words;
} * pl_readline_words_t;

typedef struct pl_readline {
  int (*pl_readline_hal_getch)();
  void (*pl_readline_hal_putch)(int ch);
  // void (*pl_readline_hal_start_cache)();
  // void (*pl_readline_hal_stop_cache)();
  list_t history;
} * pl_readline_t;

pl_readline_t pl_readline_init(int (*pl_readline_hal_getch)(),
                               void (*pl_readline_hal_putch)(int ch));
int pl_readline(_THIS, char *buffer, size_t len);