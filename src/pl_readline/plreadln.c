#include <pl_readline.h>
#define this self

static int pl_readline_add_history(_THIS, char *line) {
  if(strlen(line))
    list_prepend(this->history, strdup(line));
  return PL_READLINE_SUCCESS;
}

pl_readline_t pl_readline_init(int (*pl_readline_hal_getch)(),
                               void (*pl_readline_hal_putch)(int ch)) {
  pl_readline_t plreadln = malloc(sizeof(struct pl_readline));
  if (!plreadln)
    return NULL;
  // 设置回调函数
  plreadln->pl_readline_hal_getch = pl_readline_hal_getch;
  plreadln->pl_readline_hal_putch = pl_readline_hal_putch;
  // 设置history链表
  plreadln->history = NULL;
  return plreadln;
}
void pl_readline_uninit(_THIS) {
  list_free_with(this->history, free);
  free(this);
}
static void pl_readline_insert_char(char *str, char ch, int idx) {
  int len = strlen(str);
  memmove(str + idx + 1, str + idx, len - idx);
  str[idx] = ch;
}
static void pl_readline_delete_char(char *str, int idx) {
  int len = strlen(str);
  memmove(str + idx, str + idx + 1, len - idx);
  str[len] = '\0';
}

void pl_readline_print(_THIS, char *str) {
  while (*str) {
    this->pl_readline_hal_putch(*str++);
  }
}
static void pl_readline_reset(_THIS, int p, int len) {
  char buf[255] = {0};
  if (p) {
    sprintf(buf, "\e[%dD", p);
    pl_readline_print(this, buf);
  }
  if (len) {
    for (int i = 0; i < len; i++) {
      this->pl_readline_hal_putch(' ');
    }
    sprintf(buf, "\e[%dD", len);
    pl_readline_print(this, buf);
  }
}
static void pl_readline_to_the_end(_THIS,int n) {
  char buf[255] = {0};
  sprintf(buf, "\e[%dC", n);
  pl_readline_print(this, buf);
}
int pl_readline(_THIS, char *buffer, size_t len) {
  int p = 0;
  int length = 0;         // 输入的字符数
  int history_idx = -1;   // history的索引
  memset(buffer, 0, len); // 清空缓冲区
  while (true) {
    if (length >= len) { // 输入的字符数超过最大长度
      pl_readline_to_the_end(this, length - p);
      this->pl_readline_hal_putch('\n');
      buffer[length] = '\0';
      pl_readline_add_history(this, buffer);
      return PL_READLINE_SUCCESS;
    }
    int ch = this->pl_readline_hal_getch(); // 读取输入

    switch (ch) {
    case PL_READLINE_KEY_DOWN: {
      list_t node = list_nth(this->history, --history_idx);
      if (!node) {
        history_idx++;
        continue;
      }
      pl_readline_reset(this, p, length);
      p = 0;
      length = 0;
      memset(buffer, 0, len); // 清空缓冲区
      strcpy(buffer, node->data);
      pl_readline_print(this, buffer);
      length = strlen(buffer);
      p = length;
      break;
    }
    case PL_READLINE_KEY_UP: {
      list_t node = list_nth(this->history, ++history_idx);
      if (!node) {
        history_idx--;
        continue;
      }
      pl_readline_reset(this, p, length);
      p = 0;
      length = 0;
      memset(buffer, 0, len); // 清空缓冲区
      strcpy(buffer, node->data);
      pl_readline_print(this, buffer);
      length = strlen(buffer);
      p = length;
      break;
    }
    case PL_READLINE_KEY_LEFT:
      if (!p) // 光标在最左边
        continue;
      p--;
      pl_readline_print(this, "\e[D");
      break;
    case PL_READLINE_KEY_RIGHT:
      if (p == length) // 光标在最右边
        continue;
      p++;
      pl_readline_print(this, "\e[C");
      break;
    case PL_READLINE_KEY_BACKSPACE:
      if (!p) // 光标在最左边
        continue;
      pl_readline_delete_char(buffer, --p);
      length--;
      int n = length - p;
      if (n) {
        char buf[255] = {0};
        sprintf(buf, "\e[%dC\e[D ", n);
        pl_readline_print(this, buf);

        sprintf(buf, "\e[%dD", n);
        pl_readline_print(this, buf);
        pl_readline_print(this, "\e[D");
        pl_readline_print(this, buffer + p);
        pl_readline_print(this, buf);

      } else {
        pl_readline_print(this, "\e[D \e[D");
      }
      break;
    case PL_READLINE_KEY_ENTER:
      pl_readline_to_the_end(this, length - p);
      this->pl_readline_hal_putch('\n');
      buffer[length] = '\0';
      pl_readline_add_history(this, buffer);
      return PL_READLINE_SUCCESS;
    default: {
      pl_readline_insert_char(buffer, ch, p++);
      length++;
      int n = length - p;
      if (n) {
        char buf[255] = {0};
        pl_readline_print(this, buffer + p - 1);
        sprintf(buf, "\e[%dD", n);
        pl_readline_print(this, buf);

      } else {
        this->pl_readline_hal_putch(ch);
      }
      break;
    }
    }
  }
  return PL_READLINE_SUCCESS;
}
