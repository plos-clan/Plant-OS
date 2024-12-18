#include <pl2d.hpp>

namespace pl2d {

template <BasePixelTemplate>
auto BasePixelT::brightness() const -> T {
  return r * (FT).299 + g * (FT).587 + b * (FT).114;
}
template <BasePixelTemplate>
auto BasePixelT::grayscale() const -> BasePixelT {
  T gray = r * (FT).299 + g * (FT).587 + b * (FT).114;
  return {gray, gray, gray, a};
}
template <BasePixelTemplate>
auto BasePixelT::RGB2Grayscale() -> BasePixel & {
  T gray = r * (FT).299 + g * (FT).587 + b * (FT).114;
  r = g = b = gray;
  return *this;
}

#if FAST_COLOR_TRANSFORM
template <>
auto BasePixelBT::brightness() const -> u8 {
  return (r * 19595 + g * 38470 + b * 7471) / 65536;
}
template <>
auto BasePixelBT::grayscale() const -> BasePixelBT {
  byte gray = (r * 19595 + g * 38470 + b * 7471) / 65536;
  return BasePixelBT{gray, gray, gray, a};
}
template <>
auto BasePixelBT::RGB2Grayscale() -> BasePixel & {
  byte gray = (r * 19595 + g * 38470 + b * 7471) / 65536;
  r = g = b = gray;
  return *this;
}

template <>
auto BasePixelST::brightness() const -> u16 {
  return (r * 19595U + g * 38470U + b * 7471U) / 65536U;
}
template <>
auto BasePixelST::grayscale() const -> BasePixelST {
  u16 gray = (r * 19595U + g * 38470U + b * 7471U) / 65536U;
  return PixelS{gray, gray, gray, a};
}
template <>
auto BasePixelST::RGB2Grayscale() -> BasePixel & {
  u16 gray = (r * 19595U + g * 38470U + b * 7471U) / 65536U;
  r = g = b = gray;
  return *this;
}

template <>
auto BasePixelIT::brightness() const -> u32 {
  return (r * 19595ULL + g * 38470ULL + b * 7471ULL) / 65536ULL;
}
template <>
auto BasePixelIT::grayscale() const -> BasePixelIT {
  u32 gray = (r * 19595ULL + g * 38470ULL + b * 7471ULL) / 65536ULL;
  return PixelI{gray, gray, gray, a};
}
template <>
auto BasePixelIT::RGB2Grayscale() -> BasePixel & {
  u32 gray = (r * 19595ULL + g * 38470ULL + b * 7471ULL) / 65536ULL;
  r = g = b = gray;
  return *this;
}
#endif

BasePixelInstantiation

} // namespace pl2d
