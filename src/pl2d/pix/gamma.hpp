#include <pl2d.hpp>

// gamma 函数浮点输入范围均为 0-1
namespace pl2d::G {

// 参照 https://en.wikipedia.org/wiki/SRGB

finline auto rgb2xyz(f32 x) -> f32 {
  return (x > 0.04045f) ? cpp::pow((x + 0.055f) / 1.055f, 2.4f) : (x / 12.92f);
}
finline auto xyz2rgb(f32 x) -> f32 {
  return (x > 0.0031308f) ? (1.055f * cpp::pow(x, 1.f / 2.4f) - 0.055f) : (12.92f * x);
}
finline auto rgb2xyz(f64 x) -> f64 {
  return (x > 0.04045) ? cpp::pow((x + 0.055) / 1.055, 2.4) : (x / 12.92);
}
finline auto xyz2rgb(f64 x) -> f64 {
  return (x > 0.0031308) ? (1.055 * cpp::pow(x, 1 / 2.4) - 0.055) : (12.92 * x);
}

finline auto xyz2lab(f32 x) -> f32 {
  return (x > 0.008856f) ? cpp::cbrt(x) : ((903.3f * x + 16.f) / 116.f);
}
finline auto lab2xyz(f32 x) -> f32 {
  return (x > 0.206893f) ? cpp::cube(x) : (((x * 116.f) - 16.f) / 903.3f);
}
finline auto xyz2lab(f64 x) -> f64 {
  return (x > 0.008856) ? cpp::cbrt(x) : ((903.3f * x + 16.f) / 116.f);
}
finline auto lab2xyz(f64 x) -> f64 {
  return (x > 0.206893) ? cpp::cube(x) : (((x * 116.) - 16.) / 903.3);
}

// 输入范围 0-255 输出 0-1
#if FAST_COLOR_TRANSFORM
finline auto rgb2xyz(byte x) -> float {
#  include "rgb2xyz.lut.h"
  return lut[x];
}
finline auto xyz2rgb(byte x) -> float {
#  include "xyz2rgb.lut.h"
  return lut[x];
}
finline auto xyz2lab(byte x) -> float {
#  include "xyz2lab.lut.h"
  return lut[x];
}
finline auto lab2xyz(byte x) -> float {
#  include "lab2xyz.lut.h"
  return lut[x];
}
#else
finline auto rgb2xyz(byte x) -> float {
  return rgb2xyz(x / 255.f);
}
finline auto xyz2rgb(byte x) -> float {
  return xyz2rgb(x / 255.f);
}
finline auto xyz2lab(byte x) -> float {
  return xyz2lab(x / 255.f);
}
finline auto lab2xyz(byte x) -> float {
  return lab2xyz(x / 255.f);
}
#endif

// 输入范围 0-65535 输出 0-1
finline auto rgb2xyz(u16 x) -> float {
  return rgb2xyz(x / 65535.f);
}
finline auto xyz2rgb(u16 x) -> float {
  return xyz2rgb(x / 65535.f);
}
finline auto xyz2lab(u16 x) -> float {
  return xyz2lab(x / 65535.f);
}
finline auto lab2xyz(u16 x) -> float {
  return lab2xyz(x / 65535.f);
}

// 输入范围 0-4294967295 输出 0-1
finline auto rgb2xyz(u32 x) -> float {
  return rgb2xyz(x / 4294967295.f);
}
finline auto xyz2rgb(u32 x) -> float {
  return xyz2rgb(x / 4294967295.f);
}
finline auto xyz2lab(u32 x) -> float {
  return xyz2lab(x / 4294967295.f);
}
finline auto lab2xyz(u32 x) -> float {
  return lab2xyz(x / 4294967295.f);
}

} // namespace pl2d::G
