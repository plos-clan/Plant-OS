#include <misc.h>

dlexport int caesar_enc(char *dst, cstr src, char passwd) {
  if (passwd <= 0 || passwd >= 26) return -1;

#define s (src[i])
#define d (dst[i])
  for (int i = 0; src[i]; i++) {
    if ('A' <= s && s <= 'Z') {
      d = s + passwd;
      if (d > 'Z') dst[i] -= 26;
    } else if ('a' <= s && s <= 'z') {
      d = s + passwd;
      if (d > 'z') dst[i] -= 26;
    } else {
      d = s;
    }
  }
#undef s
#undef d

  return 0;
}

dlexport int caesar_dec(char *dst, cstr src, char passwd) {
  if (passwd <= 0 || passwd >= 26) return -1;

#define s (src[i])
#define d (dst[i])
  for (int i = 0; src[i]; i++) {
    if ('A' <= s && s <= 'Z') {
      d = s - passwd;
      if (d < 'A') dst[i] += 26;
    } else if ('a' <= s && s <= 'z') {
      d = s - passwd;
      if (d < 'a') dst[i] += 26;
    } else {
      d = s;
    }
  }
#undef s
#undef d

  return 0;
}
