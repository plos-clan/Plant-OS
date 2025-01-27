
#include <pl2d.hpp>

namespace pl2d {

template <typename T>
auto BaseTexture<T>::gaussian_blur(i32 size, f32 sigma) -> BaseTexture & {
  auto *tmp    = (PixelF *)cache::alloc(cpp::max(width, height) * sizeof(PixelF));
  f32  *kernel = (f32 *)cache::alloc(size * sizeof(f32));
  cpp::gaussian_kernel_1(kernel, size, sigma); // 生成一个一维的高斯核

  // 先进行横向卷积
  for (i32 y = 0; y < height; y++) {
    for (i32 x = 0; x < width; x++) {
      PixelF sum = 0;
      for (i32 j = 0; j < size; j++) {
        i32 index  = cpp::clamp(x - size / 2 + j, 0, (i32)width - 1);
        sum       += (PixelF)(*this)(index, y) * kernel[j];
      }
      tmp[x] = sum;
    }
    for (i32 x = 0; x < width; x++) {
      (*this)(x, y) = tmp[x];
    }
  }

  // 再进行纵向卷积
  for (i32 x = 0; x < width; x++) {
    for (i32 y = 0; y < height; y++) {
      PixelF sum = 0;
      for (i32 j = 0; j < size; j++) {
        i32 index  = cpp::clamp(y - size / 2 + j, 0, (i32)height - 1);
        sum       += (PixelF)(*this)(x, index) * kernel[j];
      }
      tmp[y] = sum;
    }
    for (i32 y = 0; y < height; y++) {
      (*this)(x, y) = tmp[y];
    }
  }

  cache::free(kernel);
  cache::free(tmp);
  return *this;
}

BaseTextureInstantiation

} // namespace pl2d
