#include <misc.h>

namespace cpp {

f64 gaussian_kernel_5[5] = {5.4488684550e-02, 2.4420134200e-01, 4.0261994689e-01, 2.4420134200e-01,
                            5.4488684550e-02};
f64 gaussian_kernel_7[7] = {4.4330481752e-03, 5.4005582622e-02, 2.4203622938e-01, 3.9905027965e-01,
                            2.4203622938e-01, 5.4005582622e-02, 4.4330481752e-03};
f64 gaussian_kernel_9[9] = {1.3383062461e-04, 4.4318616200e-03, 5.3991127421e-02,
                            2.4197144566e-01, 3.9894346936e-01, 2.4197144566e-01,
                            5.3991127421e-02, 4.4318616200e-03, 1.3383062461e-04};
f64 gaussian_kernel_11[11] = {1.4867195249e-06, 1.3383022668e-04, 4.4318484422e-03,
                              5.3990966881e-02, 2.4197072617e-01, 3.9894228312e-01,
                              2.4197072617e-01, 5.3990966881e-02, 4.4318484422e-03,
                              1.3383022668e-04, 1.4867195249e-06};
f64 gaussian_kernel_13[13] = {
    6.0758828174e-09, 1.4867195068e-06, 1.3383022505e-04, 4.4318483883e-03, 5.3990966225e-02,
    2.4197072323e-01, 3.9894227827e-01, 2.4197072323e-01, 5.3990966225e-02, 4.4318483883e-03,
    1.3383022505e-04, 1.4867195068e-06, 6.0758828174e-09};
f64 gaussian_kernel_15[15] = {
    9.1347203595e-12, 6.0758828173e-09, 1.4867195068e-06, 1.3383022505e-04, 4.4318483882e-03,
    5.3990966224e-02, 2.4197072322e-01, 3.9894227827e-01, 2.4197072322e-01, 5.3990966224e-02,
    4.4318483882e-03, 1.3383022505e-04, 1.4867195068e-06, 6.0758828173e-09, 9.1347203595e-12};

f32 gaussian_kernel_f5[5] = {5.4488684550e-02, 2.4420134200e-01, 4.0261994689e-01, 2.4420134200e-01,
                             5.4488684550e-02};
f32 gaussian_kernel_f7[7] = {4.4330481752e-03, 5.4005582622e-02, 2.4203622938e-01, 3.9905027965e-01,
                             2.4203622938e-01, 5.4005582622e-02, 4.4330481752e-03};
f32 gaussian_kernel_f9[9] = {1.3383062461e-04, 4.4318616200e-03, 5.3991127421e-02,
                             2.4197144566e-01, 3.9894346936e-01, 2.4197144566e-01,
                             5.3991127421e-02, 4.4318616200e-03, 1.3383062461e-04};
f32 gaussian_kernel_f11[11] = {1.4867195249e-06, 1.3383022668e-04, 4.4318484422e-03,
                               5.3990966881e-02, 2.4197072617e-01, 3.9894228312e-01,
                               2.4197072617e-01, 5.3990966881e-02, 4.4318484422e-03,
                               1.3383022668e-04, 1.4867195249e-06};
f32 gaussian_kernel_f13[13] = {
    6.0758828174e-09, 1.4867195068e-06, 1.3383022505e-04, 4.4318483883e-03, 5.3990966225e-02,
    2.4197072323e-01, 3.9894227827e-01, 2.4197072323e-01, 5.3990966225e-02, 4.4318483883e-03,
    1.3383022505e-04, 1.4867195068e-06, 6.0758828174e-09};
f32 gaussian_kernel_f15[15] = {
    9.1347203595e-12, 6.0758828173e-09, 1.4867195068e-06, 1.3383022505e-04, 4.4318483882e-03,
    5.3990966224e-02, 2.4197072322e-01, 3.9894227827e-01, 2.4197072322e-01, 5.3990966224e-02,
    4.4318483882e-03, 1.3383022505e-04, 1.4867195068e-06, 6.0758828173e-09, 9.1347203595e-12};

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

void gaussian_blur(f64 *dst, f64 *src, i32 len, f64 *kernel, i32 size) {
  for (i32 i = 0; i < len; i++) {
    f64 sum = 0;
    for (i32 j = 0; j < size; j++) {
      int index  = cpp::clamp(i - size / 2 + j, 0, len - 1);
      sum       += src[index] * kernel[j];
    }
    dst[i] = sum;
  }
}
void gaussian_blur(f32 *dst, f32 *src, i32 len, f32 *kernel, i32 size) {
  for (i32 i = 0; i < len; i++) {
    f32 sum = 0;
    for (i32 j = 0; j < size; j++) {
      int index  = cpp::clamp(i - size / 2 + j, 0, len - 1);
      sum       += src[index] * kernel[j];
    }
    dst[i] = sum;
  }
}

void gaussian_blur(f64 *dst, f64 *src, i32 len, i32 size, f64 sigma) {
  f64 *kernel = (f64 *)malloc(size * sizeof(f64));
  gaussian_kernel_1(kernel, size, sigma);
  gaussian_blur(dst, src, len, kernel, size);
  free(kernel);
}
void gaussian_blur(f32 *dst, f32 *src, i32 len, i32 size, f32 sigma) {
  f32 *kernel = (f32 *)malloc(size * sizeof(f32));
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
