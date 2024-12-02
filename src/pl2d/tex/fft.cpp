
#include <pl2d.hpp>

namespace pl2d {

#define FFT(_name_)                                                                                \
  template <typename T>                                                                            \
  auto BaseTexture<T>::_name_() -> BaseTexture & {                                                 \
    if (cpp::max(width, height) <= 4096) {                                                         \
      pl2d::TextureF tmp(width, height);                                                           \
      copy_to(tmp);                                                                                \
      tmp._name_();                                                                                \
      copy_from(tmp);                                                                              \
    } else {                                                                                       \
      pl2d::TextureD tmp(width, height);                                                           \
      copy_to(tmp);                                                                                \
      tmp._name_();                                                                                \
      copy_from(tmp);                                                                              \
    }                                                                                              \
    return *this;                                                                                  \
  }

FFT(fft_row)
FFT(fft_col)
FFT(fft)
FFT(ift_row)
FFT(ift_col)
FFT(ift)

#undef FFT

// float32

template <>
auto BaseTexture<PixelF>::fft_row() -> BaseTexture & {
  auto *x = (f32 *)pixels;
  for (size_t i = 0; i < height; i++) {
    fftf_r2r_p(x + pitch * 4 * i, x + pitch * 4 * i, width, 4, false);
    fftf_r2r_p(x + pitch * 4 * i + 1, x + pitch * 4 * i + 1, width, 4, false);
    fftf_r2r_p(x + pitch * 4 * i + 2, x + pitch * 4 * i + 2, width, 4, false);
    fftf_r2r_p(x + pitch * 4 * i + 3, x + pitch * 4 * i + 3, width, 4, false);
  }
  return *this;
}
template <>
auto BaseTexture<PixelF>::fft_col() -> BaseTexture & {
  auto *x = (f32 *)pixels;
  for (size_t i = 0; i < width * 4; i++) {
    fftf_r2r_p(x + i, x + i, height, pitch * 4, false);
  }
  return *this;
}
template <>
auto BaseTexture<PixelF>::fft() -> BaseTexture & {
  fft_row();
  fft_col();
  return *this;
}

template <>
auto BaseTexture<PixelF>::ift_row() -> BaseTexture & {
  auto *x = (f64 *)pixels;
  for (size_t i = 0; i < height; i++) {
    fft_r2r_p(x + pitch * 4 * i, x + pitch * 4 * i, width, 4, true);
    fft_r2r_p(x + pitch * 4 * i + 1, x + pitch * 4 * i + 1, width, 4, true);
    fft_r2r_p(x + pitch * 4 * i + 2, x + pitch * 4 * i + 2, width, 4, true);
    fft_r2r_p(x + pitch * 4 * i + 3, x + pitch * 4 * i + 3, width, 4, true);
  }
  return *this;
}
template <>
auto BaseTexture<PixelF>::ift_col() -> BaseTexture & {
  auto *x = (f64 *)pixels;
  for (size_t i = 0; i < width * 4; i++) {
    fft_r2r_p(x + i, x + i, height, pitch * 4, true);
  }
  return *this;
}
template <>
auto BaseTexture<PixelF>::ift() -> BaseTexture & {
  ift_col();
  ift_row();
  return *this;
}

// float64

template <>
auto BaseTexture<PixelD>::fft_row() -> BaseTexture & {
  auto *x = (f64 *)pixels;
  for (size_t i = 0; i < height; i++) {
    fft_r2r_p(x + pitch * 4 * i, x + pitch * 4 * i, width, 4, false);
    fft_r2r_p(x + pitch * 4 * i + 1, x + pitch * 4 * i + 1, width, 4, false);
    fft_r2r_p(x + pitch * 4 * i + 2, x + pitch * 4 * i + 2, width, 4, false);
    fft_r2r_p(x + pitch * 4 * i + 3, x + pitch * 4 * i + 3, width, 4, false);
  }
  return *this;
}
template <>
auto BaseTexture<PixelD>::fft_col() -> BaseTexture & {
  auto *x = (f64 *)pixels;
  for (size_t i = 0; i < width * 4; i++) {
    fft_r2r_p(x + i, x + i, height, pitch * 4, false);
  }
  return *this;
}
template <>
auto BaseTexture<PixelD>::fft() -> BaseTexture & {
  fft_row();
  fft_col();
  return *this;
}

template <>
auto BaseTexture<PixelD>::ift_row() -> BaseTexture & {
  auto *x = (f64 *)pixels;
  for (size_t i = 0; i < height; i++) {
    fft_r2r_p(x + pitch * 4 * i, x + pitch * 4 * i, width, 4, true);
    fft_r2r_p(x + pitch * 4 * i + 1, x + pitch * 4 * i + 1, width, 4, true);
    fft_r2r_p(x + pitch * 4 * i + 2, x + pitch * 4 * i + 2, width, 4, true);
    fft_r2r_p(x + pitch * 4 * i + 3, x + pitch * 4 * i + 3, width, 4, true);
  }
  return *this;
}
template <>
auto BaseTexture<PixelD>::ift_col() -> BaseTexture & {
  auto *x = (f64 *)pixels;
  for (size_t i = 0; i < width * 4; i++) {
    fft_r2r_p(x + i, x + i, height, pitch * 4, true);
  }
  return *this;
}
template <>
auto BaseTexture<PixelD>::ift() -> BaseTexture & {
  ift_col();
  ift_row();
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
