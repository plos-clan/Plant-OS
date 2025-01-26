#include <cmath>
#include <string>

#define NO_STD 0
#include <pl2d.hpp>
#include <plds.hpp>
#include <plui.hpp>

extern "C" {
void abort();
}

#include "qoi.h"
#include <pl2d/fb.h>

extern "C" {
void next_event();
void screen_flush();
}

enum class Event : u32 {
  ScreenResize,
  MouseMove,
  ButtonDown, // 鼠标按键按下
  ButtonUp,   // 鼠标按键释放
  KeyDown,    // 键盘按键按下
  KeyUp,      // 键盘按键释放
  Scroll,     //
};

namespace plds {

// plui::Element root;

pl2d::FrameBuffer screen_fb;
pl2d::TextureB    screen_tex;

pl2d::TextureB image_tex;
pl2d::TextureB frame_tex[19];

static auto &fb  = screen_fb;
static auto &tex = screen_tex;

template <typename T>
auto load_qoi_to_tex(const char *filename, T &tex) {
  int   img_width, img_height, img_channels;
  void *data = qoi_load(filename, &img_width, &img_height, &img_channels);
  if (data == null) abort();
  pl2d::FrameBuffer img_fb(data, img_width, img_height,
                           img_channels == 3 ? pl2d::PixFmt::RGB : pl2d::PixFmt::RGBA);
  img_fb.init_texture(tex);
  img_fb.copy_to(tex);
  free(data);
}

auto init(void *buffer, u32 width, u32 height, pl2d::PixFmt fmt) -> int {
  for (int i = 0; i < 18; i++) {
    load_qoi_to_tex(("../resource/frame" + std::to_string(i) + ".qoi").c_str(), frame_tex[i]);
    frame_tex[i].transform([](auto &pix) {
      if (pix.brightness() > 240) pix = 0;
    });
    frame_tex[i].gaussian_blur(11, 2);
  }

  load_qoi_to_tex("../resource/test.qoi", image_tex);

  return on::screen_resize(buffer, width, height, fmt);
}

int nframe = 0;

void flush() {
  nframe++;

  float        i = (f32)nframe * .01f;
  pl2d::PixelF p = {.8, cpp::cos(i) * .1f, cpp::sin(i) * .1f, 1};
  p.LAB2RGB();
  tex.fill(p);
  tex.transform([](auto &pix, i32 x, i32 y) {
    f32 k = cpp::sin((x - y + nframe * 4) / 25.f) / 5.f + .8f;
    if ((x + y) / 25 % 2 == 0) pix.mix_ratio(pl2d::PixelF{k, k, k}, 64);
  });
  frame_tex[nframe / 60 % 19].paste_to_mix(tex, 20, 20);
  image_tex.paste_to_mix(tex, 900, 0);
  tex.fill_trangle({100, 100}, {200, 200}, {100, 200},
                   pl2d::PixelF{.8, cpp::cos(i + 1.f) * .1f, cpp::sin(i + 1.f) * .1f, 1}.LAB2RGB());
  fb.flush(tex);
  screen_flush();
}

void deinit() {
  tex = {};
}

} // namespace plds

namespace plds::on {

// 屏幕大小重设
auto screen_resize(void *buffer, u32 width, u32 height, pl2d::PixFmt fmt) -> int {
  if (fb.ready) fb.reset();
  fb.pix[0] = buffer;
  fb.pixfmt = fmt;
  fb.width  = width;
  fb.height = height;
  fb.init();
  if (!fb.ready) return 1; // 初始化失败

  fb.init_texture(tex);
  if (!tex.ready()) return 1; // 初始化失败

  return 0;
}
// 鼠标移动
void mouse_move(i32 x, i32 y) {}
// 鼠标按键按下
void button_down(i32 button, i32 x, i32 y) {}
// 鼠标按键释放
void button_up(i32 button, i32 x, i32 y) {}
// 键盘按键按下
void key_down(i32 key) {}
// 键盘按键释放
void key_up(i32 key) {
  if (key == event::Escape) program_exit();
}
// 鼠标滚轮，nrows 表示内容可见区域向下 N 行
void scroll(i32 nrows) {}

} // namespace plds::on
