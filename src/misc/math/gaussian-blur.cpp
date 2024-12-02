#include <misc.h>

namespace cpp {

void gaussian_blur(f64 *dst, const f64 *src, i32 len, f64 *kernel, i32 size) {
  for (i32 i = 0; i < len; i++) {
    f64 sum = 0;
    for (i32 j = 0; j < size; j++) {
      const i32 index  = cpp::clamp(i - size / 2 + j, 0, len - 1);
      sum             += src[index] * kernel[j];
    }
    dst[i] = sum;
  }
}
void gaussian_blur(f32 *dst, const f32 *src, i32 len, f32 *kernel, i32 size) {
  for (i32 i = 0; i < len; i++) {
    f32 sum = 0;
    for (i32 j = 0; j < size; j++) {
      const i32 index  = cpp::clamp(i - size / 2 + j, 0, len - 1);
      sum             += src[index] * kernel[j];
    }
    dst[i] = sum;
  }
}

void gaussian_blur(f64 *dst, const f64 *src, i32 len, i32 size, f64 sigma) {
  var kernel = (f64 *)malloc(size * sizeof(f64));
  gaussian_kernel_1(kernel, size, sigma);
  gaussian_blur(dst, src, len, kernel, size);
  free(kernel);
}
void gaussian_blur(f32 *dst, const f32 *src, i32 len, i32 size, f32 sigma) {
  var kernel = (f32 *)malloc(size * sizeof(f32));
  gaussian_kernel_1(kernel, size, sigma);
  gaussian_blur(dst, src, len, kernel, size);
  free(kernel);
}

void gaussian_blur(f64 *data, i32 len, f64 *kernel, i32 size) {
  f64 *dst = (f64 *)malloc(len * sizeof(f64));
  gaussian_blur(dst, data, len, kernel, size);
  for (i32 i = 0; i < len; i++) {
    data[i] = dst[i];
  }
  free(dst);
}
void gaussian_blur(f32 *data, i32 len, f32 *kernel, i32 size) {
  f32 *dst = (f32 *)malloc(len * sizeof(f32));
  gaussian_blur(dst, data, len, kernel, size);
  for (i32 i = 0; i < len; i++) {
    data[i] = dst[i];
  }
  free(dst);
}

void gaussian_blur(f64 *data, i32 len, i32 size, f64 sigma) {
  f64 *dst = (f64 *)malloc(len * sizeof(f64));
  gaussian_blur(dst, data, len, size, sigma);
  for (i32 i = 0; i < len; i++) {
    data[i] = dst[i];
  }
  free(dst);
}
void gaussian_blur(f32 *data, i32 len, i32 size, f32 sigma) {
  f32 *dst = (f32 *)malloc(len * sizeof(f32));
  gaussian_blur(dst, data, len, size, sigma);
  for (i32 i = 0; i < len; i++) {
    data[i] = dst[i];
  }
  free(dst);
}

} // namespace cpp
