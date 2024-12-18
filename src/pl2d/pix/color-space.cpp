#include <pl2d.hpp>

#include "gamma.hpp"

namespace pl2d {

#if 0
void PixelB::RGB2LAB() {
  float r = G::rgb2xyz(this->r);
  float g = G::rgb2xyz(this->g);
  float b = G::rgb2xyz(this->b);

  float x = r * 0.4124564f + g * 0.3575761f + b * 0.1804375f;
  float y = r * 0.2126729f + g * 0.7151522f + b * 0.0721750f;
  float z = r * 0.0193339f + g * 0.1191920f + b * 0.9503041f;

  x /= 0.950456f;
  y /= 1.0f;
  z /= 1.088754f;

  x = G::xyz2lab(x);
  y = G::xyz2lab(y);
  z = G::xyz2lab(z);

  this->r = y * 255;       // l
  this->g = (x - y) * 127; // a
  this->b = (y - z) * 127; // b
}

void PixelB::LAB2RGB() {
  float y = this->r;     // l
  float x = y + this->g; // a
  float z = y - this->b; // b

  x = G::lab2xyz(x);
  y = G::lab2xyz(y);
  z = G::lab2xyz(z);

  x *= 0.950456f;
  y *= 1.0f;
  z *= 1.088754f;

  float r = x * 3.2404542f + y * -1.5371385f + z * -0.4985314f;
  float g = x * -0.9692660f + y * 1.8760108f + z * 0.0415560f;
  float b = x * 0.0556434f + y * -0.2040259f + z * 1.0572252f;

  this->r = G::xyz2rgb(r);
  this->g = G::xyz2rgb(g);
  this->b = G::xyz2rgb(b);
}
#endif

template <BasePixelTemplate>
auto BasePixelT::RGB2HSV() -> BasePixel & {
  FT r = this->r / (FT)T_MAX;
  FT g = this->g / (FT)T_MAX;
  FT b = this->b / (FT)T_MAX;

  FT max   = cpp::max(r, g, b);
  FT min   = cpp::min(r, g, b);
  FT delta = max - min;

  FT h = 0, s = 0, v = max;
  if (delta > (FT)1e-5) {
    s = delta / max;
    h = ((r == max) ? (g - b) : (g == max) ? (2 + b - r) : (4 + r - g)) / (delta * 6);
    if (h < 0) h += 1;
  }

  this->r = h * (FT)T_MAX;
  this->g = s * (FT)T_MAX;
  this->b = v * (FT)T_MAX;
  return *this;
}

template <BasePixelTemplate>
auto BasePixelT::HSV2RGB() -> BasePixel & {
  FT h = this->r / (FT)T_MAX * (FT)6;
  FT s = this->g / (FT)T_MAX;
  FT v = this->b / (FT)T_MAX;

  FT c = v * s;
  FT x = c * (1 - cpp::abs(cpp::mod(h, 2) - 1));
  FT m = v - c;

  FT r, g, b;
  if (h < 1) {
    r = c, g = x, b = 0;
  } else if (h < 2) {
    r = x, g = c, b = 0;
  } else if (h < 3) {
    r = 0, g = c, b = x;
  } else if (h < 4) {
    r = 0, g = x, b = c;
  } else if (h < 5) {
    r = x, g = 0, b = c;
  } else {
    r = c, g = 0, b = x;
  }

  this->r = (r + m) * (FT)T_MAX;
  this->g = (g + m) * (FT)T_MAX;
  this->b = (b + m) * (FT)T_MAX;
  return *this;
}

template <BasePixelTemplate>
auto BasePixelT::RGB2HSL() -> BasePixel & {
  FT r = this->r / (FT)T_MAX;
  FT g = this->g / (FT)T_MAX;
  FT b = this->b / (FT)T_MAX;

  FT max   = cpp::max(r, g, b);
  FT min   = cpp::min(r, g, b);
  FT delta = max - min;

  FT h = 0, s = 0, l = max + min;
  if (delta > (FT)1e-5) {
    s = delta / (l > 1 ? 2 - l : l);
    h = ((r == max) ? (g - b) : (g == max) ? (2 + b - r) : (4 + r - g)) / (delta * 6);
    if (h < 0) h += 1;
  }
  l /= 2;

  this->r = h * (FT)T_MAX;
  this->g = s * (FT)T_MAX;
  this->b = l * (FT)T_MAX;
  return *this;
}

template <BasePixelTemplate>
auto BasePixelT::HSL2RGB() -> BasePixel & {
  FT h = this->r * 6;
  FT s = this->g;
  FT l = this->b;

  FT c = (1 - cpp::abs(2 * l - 1)) * s;
  FT x = c * (1 - cpp::abs(cpp::mod(h, 2) - 1));
  FT m = l - c / 2;

  FT r, g, b;
  if (h < 1) {
    r = c, g = x, b = 0;
  } else if (h < 2) {
    r = x, g = c, b = 0;
  } else if (h < 3) {
    r = 0, g = c, b = x;
  } else if (h < 4) {
    r = 0, g = x, b = c;
  } else if (h < 5) {
    r = x, g = 0, b = c;
  } else {
    r = c, g = 0, b = x;
  }

  this->r = (r + m) * (FT)T_MAX;
  this->g = (g + m) * (FT)T_MAX;
  this->b = (b + m) * (FT)T_MAX;
  return *this;
}

// 参照 https://en.wikipedia.org/wiki/SRGB#From_sRGB_to_CIE_XYZ

template <BasePixelTemplate>
auto BasePixelT::RGB2XYZ() -> BasePixel & {
#if COLOR_RGB_LINEAR
  FT r = (FT)this->r / (FT)T_MAX;
  FT g = (FT)this->g / (FT)T_MAX;
  FT b = (FT)this->b / (FT)T_MAX;
#else
  FT r = G::rgb2xyz((FT)this->r / (FT)T_MAX);
  FT g = G::rgb2xyz((FT)this->g / (FT)T_MAX);
  FT b = G::rgb2xyz((FT)this->b / (FT)T_MAX);
#endif

  FT x = r * (FT)0.4124564 + g * (FT)0.3575761 + b * (FT)0.1804375;
  FT y = r * (FT)0.2126729 + g * (FT)0.7151522 + b * (FT)0.0721750;
  FT z = r * (FT)0.0193339 + g * (FT)0.1191920 + b * (FT)0.9503041;

  this->r = (x / (FT)0.950456) * (FT)T_MAX;
  this->g = (y / (FT)1.000000) * (FT)T_MAX;
  this->b = (z / (FT)1.088754) * (FT)T_MAX;
  return *this;
}

template <BasePixelTemplate>
auto BasePixelT::XYZ2RGB() -> BasePixel & {
  FT x = (FT)this->r / (FT)T_MAX * (FT)0.950456;
  FT y = (FT)this->g / (FT)T_MAX * (FT)1.000000;
  FT z = (FT)this->b / (FT)T_MAX * (FT)1.088754;

  FT r = x * (FT)03.2404542 + y * (FT)-1.5371385 + z * (FT)-0.4985314;
  FT g = x * (FT)-0.9692660 + y * (FT)01.8760108 + z * (FT)00.0415560;
  FT b = x * (FT)00.0556434 + y * (FT)-0.2040259 + z * (FT)01.0572252;

#if COLOR_RGB_LINEAR
  this->r = r * (FT)T_MAX;
  this->g = g * (FT)T_MAX;
  this->b = b * (FT)T_MAX;
#else
  this->r = G::xyz2rgb(r) * (FT)T_MAX;
  this->g = G::xyz2rgb(g) * (FT)T_MAX;
  this->b = G::xyz2rgb(b) * (FT)T_MAX;
#endif
  return *this;
}

// 将 RGB2XYZ XYZ2LAB 合并到一起
template <BasePixelTemplate>
auto BasePixelT::RGB2LAB() -> BasePixel & {
#if COLOR_RGB_LINEAR
  FT r = (FT)this->r / (FT)T_MAX;
  FT g = (FT)this->g / (FT)T_MAX;
  FT b = (FT)this->b / (FT)T_MAX;
#else
  FT r = G::rgb2xyz((FT)this->r / (FT)T_MAX);
  FT g = G::rgb2xyz((FT)this->g / (FT)T_MAX);
  FT b = G::rgb2xyz((FT)this->b / (FT)T_MAX);
#endif

  FT x = r * (FT)0.4124564 + g * (FT)0.3575761 + b * (FT)0.1804375;
  FT y = r * (FT)0.2126729 + g * (FT)0.7151522 + b * (FT)0.0721750;
  FT z = r * (FT)0.0193339 + g * (FT)0.1191920 + b * (FT)0.9503041;

  x /= (FT)0.950456;
  y /= (FT)1.000000;
  z /= (FT)1.088754;

  x = G::xyz2lab(x);
  y = G::xyz2lab(y);
  z = G::xyz2lab(z);

  if constexpr (T_MAX != T_MAX_2) {                // 整数
    this->r = y * (FT)T_MAX;                       // l
    this->g = (x - y) * (FT)T_MAX_2 + T_MAX_2 + 1; // a
    this->b = (y - z) * (FT)T_MAX_2 + T_MAX_2 + 1; // b
  } else {                                         // 浮点
    this->r = y;                                   // l
    this->g = x - y;                               // a
    this->b = y - z;                               // b
  }

  // 上面的代码更清晰
  // this->r = y * (FT)T_MAX;                                                // l
  // this->g = (x - y) * (FT)T_MAX_2 + (T_MAX != T_MAX_2 ? T_MAX_2 + 1 : 0); // a
  // this->b = (y - z) * (FT)T_MAX_2 + (T_MAX != T_MAX_2 ? T_MAX_2 + 1 : 0); // b
  return *this;
}

// 将 LAB2XYZ XYZ2RGB 合并到一起
template <BasePixelTemplate>
auto BasePixelT::LAB2RGB() -> BasePixel & {
  FT x, y, z;
  if constexpr (T_MAX != T_MAX_2) {                    // 整数
    y = (FT)this->r / (FT)T_MAX;                       // l
    x = y + ((FT)this->g - T_MAX_2 - 1) / (FT)T_MAX_2; // a
    z = y - ((FT)this->b - T_MAX_2 - 1) / (FT)T_MAX_2; // b
  } else {                                             // 浮点
    y = this->r;                                       // l
    x = y + this->g;                                   // a
    z = y - this->b;                                   // b
  }

  // 上面的代码更清晰
  // FT y = this->r / (FT)T_MAX;                                                    // l
  // FT x = y + ((FT)this->g - (T_MAX != T_MAX_2 ? T_MAX_2 + 1 : 0)) / (FT)T_MAX_2; // a
  // FT z = y - ((FT)this->b - (T_MAX != T_MAX_2 ? T_MAX_2 + 1 : 0)) / (FT)T_MAX_2; // b

  x = G::lab2xyz(x);
  y = G::lab2xyz(y);
  z = G::lab2xyz(z);

  x *= (FT)0.950456;
  y *= (FT)1.000000;
  z *= (FT)1.088754;

  FT r = x * (FT)03.2404542 + y * (FT)-1.5371385 + z * (FT)-0.4985314;
  FT g = x * (FT)-0.9692660 + y * (FT)01.8760108 + z * (FT)00.0415560;
  FT b = x * (FT)00.0556434 + y * (FT)-0.2040259 + z * (FT)01.0572252;

#if COLOR_RGB_LINEAR
  this->r = r * (FT)T_MAX;
  this->g = g * (FT)T_MAX;
  this->b = b * (FT)T_MAX;
#else
  this->r = G::xyz2rgb(r) * (FT)T_MAX;
  this->g = G::xyz2rgb(g) * (FT)T_MAX;
  this->b = G::xyz2rgb(b) * (FT)T_MAX;
#endif
  return *this;
}

//;  未实现
template <BasePixelTemplate>
auto BasePixelT::XYZ2LUV() -> BasePixel & {
  float x = this->r;
  float y = this->g;
  float z = this->b;

  float L = (y > 0.008856f) ? 116.f * cpp::cbrt(y) - 16.f : (903.3f * y + 16.f) / 116.f;

  float u_prime = 4.f * x / (x + 15.f * y + 3.f * z);
  float v_prime = 9.f * y / (x + 15.f * y + 3.f * z);

  this->r = L;       // L component
  this->g = u_prime; // u' component
  this->b = v_prime; // v' component
  return *this;
}

template <BasePixelTemplate>
auto BasePixelT::RGB2LUV() -> BasePixel & {
  TODO();
  return *this;
}
template <BasePixelTemplate>
auto BasePixelT::LUV2RGB() -> BasePixel & {
  TODO();
  return *this;
}

BasePixelInstantiation

} // namespace pl2d
