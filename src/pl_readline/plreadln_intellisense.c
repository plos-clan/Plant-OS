#include <pl_readline.h>


static void insert_char(_SELF, char ch, pl_readline_runtime *rt) {

  pl_readline_insert_char_and_view(self, ch, rt);
  pl_readline_insert_char(rt->input_buf, ch, rt->input_buf_ptr++);
}
static void insert_string(_SELF, char *str, pl_readline_runtime *rt) {
  while (*str) {
    insert_char(self, *str++, rt);
  }
}
pl_readline_word pl_readline_intellisense(_SELF, pl_readline_runtime *rt,
                                          pl_readline_words_t words) {
  char *buf;

  char *the_word = NULL;
  int times = 0;
  int idx;
  int flag = 0;
  if (rt->intellisense_mode == false) {
    buf = strdup(rt->input_buf);
    rt->intellisense_word = buf;
    buf[rt->input_buf_ptr] = '\0';
    idx = rt->input_buf_ptr;
  } else {
    assert(rt->intellisense_word != NULL);
    buf = rt->intellisense_word;
    idx = strlen(buf);
  }
  self->pl_readline_get_words(buf, words);
  int can_be_selected = 0;
  for (int i = 0; i < words->len; i++) {
    if (strncmp(buf, words->words[i].word, idx) == 0) {
      if (strlen(words->words[i].word) > rt->input_buf_ptr)
        can_be_selected++;
      if (strlen(words->words[i].word) == rt->input_buf_ptr) {
        char *buf1 = strdup(rt->input_buf);
        buf1[rt->input_buf_ptr] = '\0';
        if (strcmp(words->words[i].word, buf1) == 0) {
          flag = 1;
        } else {
          can_be_selected++;
        }
        free(buf1);
      }
    }
  }
  if (can_be_selected == 0 && flag == 1) {
    return (pl_readline_word){0};
  }
  for (int i = 0; i < words->len; i++) {
    if (strncmp(buf, words->words[i].word, idx) == 0) {
      if (!rt->intellisense_mode && strlen(words->words[i].word) == idx) {
        continue;
      }
      if (!rt->intellisense_mode) {
        if(the_word == NULL || strlen(words->words[i].word) < strlen(the_word))
          the_word = words->words[i].word;
      } else {
        if (times == 0) {
          pl_readline_next_line(self, rt);
        }
        if (times)
          pl_readline_print(self, " ");
        pl_readline_print(self, words->words[i].word);
        times++;
      }
    }
  }
  pl_readline_word ret = {0};
  if (rt->intellisense_mode == false) {
    ret.word = the_word;
    rt->intellisense_mode = true;
  }
  if (rt->intellisense_mode && times) {
    ret.first = true;
  }

  return ret;
}
void pl_readline_intellisense_insert(_SELF, pl_readline_runtime *rt,
                                     pl_readline_word word) {
  insert_string(self, word.word + rt->input_buf_ptr, rt);
}