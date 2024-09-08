#include <plty.h>

finline int color_diff(color_t c1, color_t c2) {
  int d1 = c1.r - c2.r;
  int d2 = c1.g - c2.g;
  int d3 = c1.b - c2.b;
  return d1 * d1 + d2 * d2 + d3 * d3;
}

static int color_best_match16(color_t c) {
  int best_id  = 0;
  int diff_min = color_diff(c, color_table_16[0]);
#pragma unroll
  for (int i = 1; i < lengthof(color_table_16); i++) {
    int diff = color_diff(c, color_table_16[i]);
    if (diff < diff_min) {
      best_id  = i;
      diff_min = diff;
    }
  }
  return best_id;
}
