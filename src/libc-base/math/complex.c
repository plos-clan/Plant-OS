// This code is released under the MIT License

#include <libc-base.h>

dlexport cf32 __mulsc3(f32 a, f32 b, f32 c, f32 d) {
  return (cf32){a * c - b * d, a * d + b * c};
}

dlexport cf64 __muldc3(f64 a, f64 b, f64 c, f64 d) {
  return (cf64){a * c - b * d, a * d + b * c};
}

dlexport cf32 __divsc3(f32 a, f32 b, f32 c, f32 d) {
  f32 e = c * c + d * d;
  return (cf32){(a * c + b * d) / e, (b * c - a * d) / e};
}

dlexport cf64 __divdc3(f64 a, f64 b, f64 c, f64 d) {
  f64 e = c * c + d * d;
  return (cf64){(a * c + b * d) / e, (b * c - a * d) / e};
}
