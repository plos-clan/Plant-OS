// This code is released under the MIT License

#include <libc-base.h>

// 你 builtin 一下这么难是吧

#if NO_STD

dlexport cf32 __mulsc3(f32 a, f32 b, f32 c, f32 d) {
  return (a * c - b * d) + (a * d + b * c) * 1.if;
}

dlexport cf64 __muldc3(f64 a, f64 b, f64 c, f64 d) {
  return (a * c - b * d) + (a * d + b * c) * 1.i;
}

dlexport cf32 __divsc3(f32 a, f32 b, f32 c, f32 d) {
  f32 e = c * c + d * d;
  return (a * c + b * d) / e + (b * c - a * d) / e * 1.if;
}

dlexport cf64 __divdc3(f64 a, f64 b, f64 c, f64 d) {
  f64 e = c * c + d * d;
  return (a * c + b * d) / e + (b * c - a * d) / e * 1.i;
}

#endif
