
#include <pl2d.hpp>

namespace pl2d {

#if BETTER_COLOR_INTERPOLATE && FAST_COLOR_INTERPOLATE
#  warning "启用 BETTER_COLOR_INTERPOLATE 的情况下启用 FAST_COLOR_INTERPOLATE 也快不到哪去"
#endif

#if 0
template <typename T>
void color_lerp(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
  for (size_t i = 0; i < n; i++) {
    f32 k    = (f32)i / (f32)(n - 1);
    buf[i].r = (1 - k) * src.r + k * dst.r;
    buf[i].g = (1 - k) * src.g + k * dst.g;
    buf[i].b = (1 - k) * src.b + k * dst.b;
    buf[i].a = (1 - k) * src.a + k * dst.a;
  }
}
#endif

template <typename T>
void color_lerp(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
  for (size_t i = 0; i < n; i++) {
    val k  = (f32)i / (f32)(n - 1);
    buf[i] = (1 - k) * src + k * dst;
  }
}

// 我不知道这个是否真的有效
#if FAST_COLOR_INTERPOLATE
template <>
void color_lerp<PixelB>(PixelB *buf, size_t n, PixelB src, PixelB dst) {
  if (buf == null || n == 0) return;
  for (size_t i = 0; i < n; i++) {
    i32 k    = i * 65536 / (i32)(n - 1);
    buf[i].r = ((65536 - k) * src.r + k * dst.r) / 65536;
    buf[i].g = ((65536 - k) * src.g + k * dst.g) / 65536;
    buf[i].b = ((65536 - k) * src.b + k * dst.b) / 65536;
    buf[i].a = ((65536 - k) * src.a + k * dst.a) / 65536;
  }
}
#endif

// 开启 BETTER_COLOR_INTERPOLATE 的情况下使用 LAB 空间进行插值
// 否则使用 RGB 空间
template <typename T>
void color_interpolate(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
#if BETTER_COLOR_INTERPOLATE
  src.RGB2LAB();
  dst.RGB2LAB();
#endif

  color_lerp(buf, n, src, dst);

#if BETTER_COLOR_INTERPOLATE
  for (size_t i = 0; i < n; i++) {
    buf[i].LAB2RGB();
  }
#endif
}

template <typename T>
void color_hsv_interpolate(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
  src.RGB2HSV();
  dst.RGB2HSV();

  color_lerp(buf, n, src, dst);

  for (size_t i = 0; i < n; i++) {
    buf[i].HSV2RGB();
  }
}

template <typename T>
void color_hsl_interpolate(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
  src.RGB2HSL();
  dst.RGB2HSL();

  color_lerp(buf, n, src, dst);

  for (size_t i = 0; i < n; i++) {
    buf[i].HSL2RGB();
  }
}

template <typename T>
void color_lab_interpolate(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
  src.RGB2LAB();
  dst.RGB2LAB();

  color_lerp(buf, n, src, dst);

  for (size_t i = 0; i < n; i++) {
    buf[i].LAB2RGB();
  }
}

template <typename T>
void color_luv_interpolate(T *buf, size_t n, T src, T dst) {
  if (buf == null || n == 0) return;
  src.RGB2LUV();
  dst.RGB2LUV();

  color_lerp(buf, n, src, dst);

  for (size_t i = 0; i < n; i++) {
    buf[i].LUV2RGB();
  }
}

// 依托答辩 别管
#define INSTANTIATION(_name_)                                                                      \
  template void _name_(PixelB *buf, size_t n, PixelB src, PixelB dst);                             \
  template void _name_(PixelS *buf, size_t n, PixelS src, PixelS dst);                             \
  template void _name_(PixelI *buf, size_t n, PixelI src, PixelI dst);                             \
  template void _name_(PixelF *buf, size_t n, PixelF src, PixelF dst);                             \
  template void _name_(PixelD *buf, size_t n, PixelD src, PixelD dst);

INSTANTIATION(color_lerp)
INSTANTIATION(color_interpolate)
INSTANTIATION(color_hsv_interpolate)
INSTANTIATION(color_hsl_interpolate)
INSTANTIATION(color_lab_interpolate)
INSTANTIATION(color_luv_interpolate)

#undef INSTANTIATION

// namespace color {

// class InterpolateRect {
//   i32    x1, y1, x2, y2 ;
//   PixelF topleft, topright, bottomleft, bottomright;

// public:
//   InterpolateRect() = delete;
//   InterpolateRect(i32 w, i32 h) : x1(0), y1(0), x2(w - 1), y2(h - 1) {}
//   InterpolateRect(i32 x1, i32 y1, i32 x2, i32 y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

//   class Iterator {
//   private:
//     i32 x1, y1, x2, y2;
//     i32 x, y;

//   public:
//     Iterator() = delete;
//     Iterator(i32 x1, i32 y1, i32 x2, i32 y2) : x1(x1), y1(y1), x2(x2), y2(y2), x(x1), y(y1) {}
//     Iterator(i32 x1, i32 y1, i32 x2, i32 y2, i32 x, i32 y)
//         : x1(x1), y1(y1), x2(x2), y2(y2), x(x), y(y) {}
//     Iterator(const Iterator &)                         = default;
//     Iterator(Iterator &&) noexcept                     = default;
//     auto operator=(const Iterator &) -> Iterator     & = default;
//     auto operator=(Iterator &&) noexcept -> Iterator & = default;

//     auto operator*() -> Point2I {
//       return {x, y};
//     }

//     auto operator++() -> Iterator & {
//       x++;
//       if (x > x2) {
//         x = x1;
//         y++;
//       }
//       return *this;
//     }

//     auto operator==(const Iterator &it) const -> bool {
//       return y == it.y && x == it.x;
//     }

//     auto operator!=(const Iterator &it) const -> bool {
//       return y != it.y || x != it.x;
//     }
//   };

//   auto begin() const -> Iterator {
//     return {x1, y1, x2, y2, x1, y1};
//   }

//   auto end() const -> Iterator {
//     return {x1, y1, x2, y2, x1, y2 + 1};
//   }
// };

// } // namespace color

// void pd2d_color_interpolate(color_t *restrict colors, int ncolors, color_t src, color_t dst) {
//   if (interpolate_mode_lab) {
//     RGB2LAB(&src);
//     RGB2LAB(&dst);
//   }

//   for (int i = 0; i < ncolors; i++) {
//     float k     = (float)i / (ncolors - 1);
//     colors[i].r = (1 - k) * src.r + k * dst.r;
//     colors[i].g = (1 - k) * src.g + k * dst.g;
//     colors[i].b = (1 - k) * src.b + k * dst.b;
//     colors[i].a = (1 - k) * src.a + k * dst.a;
//   }

//   if (interpolate_mode_lab) {
//     for (int i = 0; i < ncolors; i++) {
//       LAB2RGB(colors + i);
//     }
//   }
// }

} // namespace pl2d
