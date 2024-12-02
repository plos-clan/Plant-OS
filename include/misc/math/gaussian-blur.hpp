#pragma once
#include "../00-include.hpp"

namespace cpp {

dlimport f64 gaussian_kernel_5[5];
dlimport f64 gaussian_kernel_7[7];
dlimport f64 gaussian_kernel_9[9];
dlimport f64 gaussian_kernel_11[11];
dlimport f64 gaussian_kernel_13[13];
dlimport f64 gaussian_kernel_15[15];

dlimport f32 gaussian_kernel_f5[5];
dlimport f32 gaussian_kernel_f7[7];
dlimport f32 gaussian_kernel_f9[9];
dlimport f32 gaussian_kernel_f11[11];
dlimport f32 gaussian_kernel_f13[13];
dlimport f32 gaussian_kernel_f15[15];

// 一维高斯模糊卷积核
dlimport void gaussian_kernel_1(f64 *kernel, i32 size, f64 sigma);
dlimport void gaussian_kernel_1(f32 *kernel, i32 size, f32 sigma);

// 二维高斯模糊卷积核
dlimport void gaussian_kernel_2(f64 *kernel, i32 size, f64 sigma);
dlimport void gaussian_kernel_2(f32 *kernel, i32 size, f32 sigma);

dlimport void gaussian_blur(f64 *dst, const f64 *src, i32 len, f64 *kernel, i32 size);
dlimport void gaussian_blur(f32 *dst, const f32 *src, i32 len, f32 *kernel, i32 size);

dlimport void gaussian_blur(f64 *dst, const f64 *src, i32 len, i32 size, f64 sigma);
dlimport void gaussian_blur(f32 *dst, const f32 *src, i32 len, i32 size, f32 sigma);

dlimport void gaussian_blur(f64 *data, i32 len, f64 *kernel, i32 size);
dlimport void gaussian_blur(f32 *data, i32 len, f32 *kernel, i32 size);

dlimport void gaussian_blur(f64 *data, i32 len, i32 size, f64 sigma);
dlimport void gaussian_blur(f32 *data, i32 len, i32 size, f32 sigma);

} // namespace cpp
