#include <iostream>

#define NO_STD 0
#include <misc.h>

// 一维高斯模糊卷积核
void gaussian_kernel_1(f64 *kernel, i32 size, f64 sigma) {
  i32 center = size / 2;
  f64 sum    = 0;
  f64 k      = -1 / (2 * sigma * sigma);
  for (i32 i = 0; i < size; i++) {
    const i32 x  = i - center;
    kernel[i]    = cpp::exp((x * x) * k);
    sum         += kernel[i];
  }
  for (i32 i = 0; i < size; i++) {
    kernel[i] /= sum;
  }
}
void gaussian_kernel_1(f32 *kernel, i32 size, f32 sigma) {
  i32 center = size / 2;
  f32 sum    = 0;
  f32 k      = -1 / (2 * sigma * sigma);
  for (i32 i = 0; i < size; i++) {
    const i32 x  = i - center;
    kernel[i]    = cpp::exp((x * x) * k);
    sum         += kernel[i];
  }
  for (i32 i = 0; i < size; i++) {
    kernel[i] /= sum;
  }
}

// 二维高斯模糊卷积核
void gaussian_kernel_2(f64 *kernel, i32 size, f64 sigma) {
  i32 center = size / 2;
  f64 sum    = 0;
  f64 k      = -1 / (2 * sigma * sigma);
  for (i32 i = 0; i < size; i++) {
    for (i32 j = 0; j < size; j++) {
      const i32 x = i - center, y = j - center;
      kernel[i * size + j]  = cpp::exp((x * x + y * y) * k);
      sum                  += kernel[i * size + j];
    }
  }
  for (i32 i = 0; i < size * size; i++) {
    kernel[i] /= sum;
  }
}
void gaussian_kernel_2(f32 *kernel, i32 size, f32 sigma) {
  i32 center = size / 2;
  f32 sum    = 0;
  f32 k      = -1 / (2 * sigma * sigma);
  for (i32 i = 0; i < size; i++) {
    for (i32 j = 0; j < size; j++) {
      const i32 x = i - center, y = j - center;
      kernel[i * size + j]  = cpp::exp((x * x + y * y) * k);
      sum                  += kernel[i * size + j];
    }
  }
  for (i32 i = 0; i < size * size; i++) {
    kernel[i] /= sum;
  }
}

auto main() -> int {
  for (int n = 3; n <= 15; n += 2) {
    f64 k[n];
    gaussian_kernel_1(k, n, 1);
    for (int i = 0; i < n; i++) {
      printf("%.10e, ", k[i]);
    }
    std::cout << std::endl;
  }
}
