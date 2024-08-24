#include "define/type/base.h"
#include "define/type/bool.h"
#include "kernel/logging.h"
#include "libc-base/string/str.h"
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
// @return : the prefix of the words in the words list
static char *get_the_same_prefix_in_words(_SELF, pl_readline_words_t words) {
  // return a pointer has been malloced
  // use strndup to copy the prefix
  char *prefix = NULL;
  int len = 0;
  for (int i = 0; i < words->len; i++) {
    if (prefix == NULL) {
      prefix = strdup(words->words[i].word);
      len = strlen(prefix);
    } else {
      int j = 0;
      while (j < len && j < strlen(words->words[i].word) &&
             prefix[j] == words->words[i].word[j]) {
        j++;
      }
      if (j < len) {
        prefix = realloc(prefix, j + 1);
        len = j;
        prefix[j] = '\0';
      }
    }
  }
  return prefix;
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

  pl_readline_words_t words_temp = NULL;
  if(!rt->intellisense_mode) {
    words_temp = pl_readline_word_maker_init();
  }
  for (int i = 0; i < words->len; i++) {
    if (strncmp(buf, words->words[i].word, idx) == 0) {
      if (!rt->intellisense_mode && strlen(words->words[i].word) == idx) {
        continue;
      }
      if (!rt->intellisense_mode) {
        pl_readline_word_maker_add(words->words[i].word,words_temp ,false);
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
    char *prefix = get_the_same_prefix_in_words(self, words_temp);
    if(strcmp(prefix,buf) == 0) {
      free(prefix);
      prefix = strdup(the_word);
    }
    ret.word = prefix;
    rt->intellisense_mode = true;
    pl_readline_word_maker_destroy(words_temp);
  }
  if (rt->intellisense_mode && times) {
    ret.first = true;
  }

  return ret;
}
void pl_readline_intellisense_insert(_SELF, pl_readline_runtime *rt,
                                     pl_readline_word word) {
  insert_string(self, word.word + rt->input_buf_ptr, rt);
  free(word.word);
}