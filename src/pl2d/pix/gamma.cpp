#include <pl2d.hpp>

namespace pl2d {

// 参照 https://en.wikipedia.org/wiki/SRGB

auto gamma_correct(f32 x) -> f32 {
  return (x > 0.0031308f) ? (1.055f * cpp::pow(x, 1.f / 2.4f) - 0.055f) : (12.92f * x);
}
auto gamma_correct(f64 x) -> f64 {
  return (x > 0.0031308) ? (1.055 * cpp::pow(x, 1 / 2.4) - 0.055) : (12.92 * x);
}
auto reverse_gamma(f32 x) -> f32 {
  return (x > 0.04045f) ? cpp::pow((x + 0.055f) / 1.055f, 2.4f) : (x / 12.92f);
}
auto reverse_gamma(f64 x) -> f64 {
  return (x > 0.04045) ? cpp::pow((x + 0.055) / 1.055, 2.4) : (x / 12.92);
}

template <BasePixelTemplate>
auto BasePixelT::gamma_correct() const -> BasePixel {
  T _r = pl2d::gamma_correct((FT)r / (FT)T_MAX) * (FT)T_MAX;
  T _g = pl2d::gamma_correct((FT)r / (FT)T_MAX) * (FT)T_MAX;
  T _b = pl2d::gamma_correct((FT)r / (FT)T_MAX) * (FT)T_MAX;
  T _a = pl2d::gamma_correct((FT)r / (FT)T_MAX) * (FT)T_MAX;
  return {_r, _g, _b, _a};
}
template <BasePixelTemplate>
auto BasePixelT::reverse_gamma() const -> BasePixel {
  T _r = pl2d::reverse_gamma((FT)r / (FT)T_MAX) * (FT)T_MAX;
  T _g = pl2d::reverse_gamma((FT)r / (FT)T_MAX) * (FT)T_MAX;
  T _b = pl2d::reverse_gamma((FT)r / (FT)T_MAX) * (FT)T_MAX;
  T _a = pl2d::reverse_gamma((FT)r / (FT)T_MAX) * (FT)T_MAX;
  return {_r, _g, _b, _a};
}

BasePixelInstantiation

} // namespace pl2d
