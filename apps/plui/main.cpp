// This code is released under the MIT License

#include <pl2d.hpp>
#include <sys.h>

pl2d::FrameBuffer fb;
pl2d::TextureB    tex;

int main() {
  void *vram;
  int   width, height;
  __syscall(SYSCALL_VBE_GETMODE, &vram, &width, &height);
  printf("vram=%p, width=%d, height=%d\n", vram, width, height);
  fb.pix[0] = vram;
  fb.pixfmt = pl2d::PixFmt::RGBA;
  fb.width  = width;
  fb.height = height;
  fb.init();
  fb.init_texture(tex);
  tex.fill(0x00ff00ff);
  fb.flush(tex);

  while (true) {
    static int nframe = 0;
    nframe++;
    float        i = (f32)nframe * .01f;
    pl2d::PixelF p = {.8, cpp::cos(i) * .1f, cpp::sin(i) * .1f, 1};
    p.LAB2RGB();
    tex.fill(p);
    tex.transform([](auto &pix, i32 x, i32 y) {
      f32 k = cpp::sin((x - y + nframe * 4) / 25.f) / 5.f + .8f;
      if ((x + y) / 25 % 2 == 0) pix.mix_ratio(pl2d::PixelF{k, k, k}, 64);
    });
    fb.flush(tex);
  }

  return 0;
}
