#include <kernel.h>

extern struct tty *tty_default;

void screen_clear() {
  tty_default->clear(tty_default);
  gotoxy(0, 0);
}

void printchar(char ch) {
  char ch1[2] = {ch, '\0'};
  tty_default->print(tty_default, ch1);
}

static const char eos[] = {'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'f', 'J', 'K', 'S', 'T', 'm'};
// end of string,
// vt100控制字符中可能的结束符号

finline bool t_is_eos(char ch) {
#pragma unroll
  for (size_t i = 0; i < lengthof(eos); i++) {
    if (ch == eos[i]) return true;
  }
  return false;
}

static int parse_vt100(struct tty *res, char *string) {
  switch (res->mode) {
  case MODE_A: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, res->x, res->y - delta);
    return 1;
  }
  case MODE_B: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, res->x, res->y + delta);
    return 1;
  }
  case MODE_C: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, res->x + delta, res->y);
    return 1;
  }
  case MODE_D: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, res->x - delta, res->y);
    return 1;
  }
  case MODE_E: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, 0, res->y + delta);
    return 1;
  }
  case MODE_F: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, 0, res->y - delta);
    return 1;
  }
  case MODE_G: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (!delta) delta = 1;
    res->gotoxy(res, delta - 1, res->y); // 可能是这样的
    return 1;
  }
  case MODE_H: {
    int  k                 = 0;
    char dig_string[2][81] = {};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) {
        if (string[i] == ';') { // 分号
          k++;
          j = 0;
          continue;
        }
        return 0;
      }
      dig_string[k][j++] = string[i];
    }
    if (k > 1) return 0;

    int delta[2] = {0};
    for (int i = 0; i <= k; i++) {
      delta[i] = atol(dig_string[i]);
    }
    if (k == 0 && delta[0] != 0) return 0;
    switch (k) {
    case 0: res->gotoxy(res, 0, 0); break;
    case 1: res->gotoxy(res, delta[0], delta[1]); break;
    default: return 0;
    }
    return 1;
  }
  case MODE_J: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    int old_x = res->x, old_y = res->y;
    switch (delta) {
    case 0: {
      int flag = 0;
      for (int i = old_y * res->xsize + old_x; i < res->xsize * res->ysize; i++) {
        flag = 1;
        res->putchar(res, ' ');
      }
      if (flag) res->gotoxy(res, old_x, old_y);
      break;
    }
    case 1:
      res->gotoxy(res, 0, 0);
      for (int i = 0; i < old_y * res->xsize + old_x; i++) {
        res->putchar(res, ' ');
      }
      break;
    case 2: res->clear(res); break;
    default: return 0;
    }
    return 1;
  }
  case MODE_K: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    int old_x = res->x, old_y = res->y;
    switch (delta) {
    case 0: {
      int flag = 0;
      for (int i = old_y * res->xsize + old_x; i < (old_y + 1) * res->ysize; i++) {
        flag = 1;
        res->putchar(res, ' ');
      }
      if (flag) res->gotoxy(res, old_x, old_y);
      break;
    }
    case 1:
      res->gotoxy(res, 0, res->y);
      for (int i = old_y * res->xsize; i < old_y * res->xsize + old_x; i++) {
        res->putchar(res, ' ');
      }
      break;
    case 2:
      res->gotoxy(res, 0, res->y);
      for (int i = old_y * res->xsize; i < (old_y + 1) * res->xsize; i++) {
        res->putchar(res, ' ');
      }
      res->gotoxy(res, old_x, old_y);
      break;
    default: return 0;
    }
    return 1;
  }
  case MODE_S: return 0; // unsupported
  case MODE_T: {
    char dig_string[81] = {0};
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) return 0;
      dig_string[j++] = string[i];
    }
    int delta = atol(dig_string);
    if (delta) return 0;
    res->screen_ne(res);
    return 1;
  }
  case MODE_m: {
    // klogd("MODE_m");
    int  k = 0;
    char dig_string[2][81];
    memset(dig_string, 0, sizeof(dig_string)); // 全部设置为0
    for (int i = 2, j = 0; string[i]; i++) {
      if (t_is_eos(string[i])) break;
      if (!isdigit(string[i])) {
        if (string[i] == ';') { // 分号
          k++;
          j = 0;
          continue;
        }
        kloge("nan");
        return 0;
      }
      dig_string[k][j++] = string[i];
    }
    if (k > 1) {
      kloge("k > 1");
      return 0;
    }

    int delta[2] = {0};
    // klogd("start for %d", k);
    for (int i = 0; i <= k; i++) {
      delta[i] = atol(dig_string[i]);
    }
    if (delta[0] == 0 && k == 0) {
      int sel_color    = res->color_saved != -1 ? res->color_saved : res->color;
      res->color       = sel_color;
      res->color_saved = -1;
      return 1;
    } else if (delta[0] == 1 && k == 0) { // unsupported
      kloge("unsupport");
      return 0;
    }
    // klogd("switch k");
    static const byte color_map[8] = {0, 4, 2, 6, 1, 5, 3, 7};
    switch (k) {
    case 0: {
      if (delta[0] >= 30 && delta[0] <= 37) { // foreground color
        if (res->color_saved == -1) res->color_saved = res->color;
        res->color &= 0xf0;
        res->color |= color_map[delta[0] - 30];
        return 1;
      } else if (delta[0] >= 40 && delta[0] <= 47) {
        if (res->color_saved == -1) res->color_saved = res->color;
        res->color &= 0x0f;
        res->color |= color_map[delta[0] - 40] << 4;
        return 1;
      } else {
        kloge("delta error %d", delta);
        return 0;
      }
    }
    case 1: {
      if (delta[0] != 1) {
        kloge("not 1");
        return 0;
      }
      if (delta[1] >= 30 && delta[1] <= 37) { // foreground color
        if (res->color_saved == -1) { res->color_saved = res->color; }
        res->color &= 0xf0;
        res->color |= color_map[delta[1] - 30] + 8;
        return 1;
      } else if (delta[1] >= 40 && delta[1] <= 47) {
        if (res->color_saved == -1) res->color_saved = res->color;
        res->color &= 0x0f;
        res->color |= (color_map[delta[1] - 40] + 8) << 4;
        return 1;
      } else {
        kloge("delta error0 %d", delta);
        return 0;
      }
    }
    default: return 0;
    }
  }
  default: break;
  }
  return 0;
}

void t_putchar(struct tty *res, char ch) {
  if (ch == '\033' && res->vt100 == 0) {
    // klogd("vt100");
    memset(res->buffer, 0, 81);
    res->buf_p                = 0;
    res->buffer[res->buf_p++] = '\033';
    res->vt100                = 1;
    res->done                 = 0;
    return;
  } else if (res->vt100 && res->buf_p == 1) {
    if (ch == '[') {
      res->buffer[res->buf_p++] = ch;
      return;
    } else {
      res->vt100 = 0;
      for (int i = 0; i < res->buf_p; i++) {
        res->putchar(res, res->buffer[i]);
      }
    }
  } else if (res->vt100 && res->buf_p == 81) {
    for (int i = 0; i < res->buf_p; i++) {
      res->putchar(res, res->buffer[i]);
    }
    res->vt100 = 0;
  } else if (res->vt100) {
    res->buffer[res->buf_p++] = ch;
    if (t_is_eos(ch)) {
      res->mode = (vt100_mode_t)ch;
      if (!parse_vt100(res, res->buffer)) { // 失败了
        klogd("failed to parse");
        for (int i = 0; i < res->buf_p; i++) {
          res->putchar(res, res->buffer[i]);
        }
      }
      res->vt100 = 0;
      return;
    } else if (!isdigit(ch) && ch != ';') {
      for (int i = 0; i < res->buf_p; i++) {
        res->putchar(res, res->buffer[i]);
      }
      res->vt100 = 0;
      return;
    }

    return;
  }
  res->putchar(res, ch);
}

void putchar(int ch) {
  t_putchar(tty_default, ch);
}

void screen_flush() {
  tty_default->flush(tty_default);
}

void screen_ne() {
  tty_default->screen_ne(tty_default);
}

int get_x() {
  return tty_default->x;
}

int get_y() {
  return tty_default->y;
}

int get_raw_y() {
  return tty_default->Raw_y;
}

int get_xsize() {
  return tty_default->xsize;
}

int get_ysize() {
  return tty_default->ysize;
}

void print(cstr str) {
  tty_default->print(tty_default, str);
}

void gotoxy(int x1, int y1) {
  tty_default->gotoxy(tty_default, x1, y1);
}
