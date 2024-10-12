#include <kernel.h>

#pragma GCC optimize("O0")

#pragma clang optimize off

#define rmfarptr2ptr(x) ((void *)((x).seg * 0x10 + (x).offset))

static int now_width = 0, now_height = 0;
static int now_xoff = 0, now_yoff = 0;

void *vbe_frontbuffer;
void *vbe_backbuffer;

#if 0
void SwitchToText8025_BIOS() {
  regs16 regs = {.ax = 0x0003};
  asm16_int(0x10, &regs);
}

void SwitchTo320X200X256_BIOS() {
  regs16 regs;
  regs.ax = 0x0013;
  asm16_int(0x10, &regs);
}
#endif

char *GetSVGACharOEMString() {
  regs16 regs = {.ax = 0x4f00, .es = 0x07e0, .di = 0x0000};
  asm16_int(0x10, &regs);
  VESAControllerInfo *info = (VESAControllerInfo *)VBEINFO_ADDR;
  return rmfarptr2ptr(info->oemString);
}

int vbe_get_controller_info(VESAControllerInfo *addr) {
  regs16 r = {.ax = 0x4f00, .es = (size_t)addr / 16, .di = (size_t)addr % 16};
  asm16_int(0x10, &r);
  return r.ax;
}

int vbe_get_mode_info(u16 mode, VESAModeInfo *addr) {
  mode     |= MASK(14);
  regs16 r  = {.ax = 0x4f01, .cx = mode, .es = (size_t)addr / 16, .di = (size_t)addr % 16};
  asm16_int(0x10, &r);
  return r.ax;
}

void *vbe_set_mode(u16 mode) {
  mode        |= MASK(14);
  var    addr  = (VESAModeInfo *)VBEMODEINFO_ADDR;
  regs16 r1    = {.ax = 0x4f01, .cx = mode, .es = (size_t)addr / 16, .di = (size_t)addr % 16};
  asm16_int(0x10, &r1);
  if (r1.ax != 0x004f) fatal("Error when vbe_set_mode");
  regs16 r2 = {.ax = 0x4f02, .bx = mode};
  asm16_int(0x10, &r2);
  if (r2.ax != 0x004f) fatal("Error when vbe_set_mode");
  now_width       = addr->width;
  now_height      = addr->height;
  vbe_frontbuffer = (void *)addr->physbase;
  vbe_backbuffer  = (void *)addr->physbase + addr->width * addr->height * addr->bitsPerPixel / 8;
  return vbe_frontbuffer;
}

// 获取所有支持的模式
// 在屏幕上打印，模式号，分辨率，色深
void vbe_print_all_modes() {
  var vbe   = (VESAControllerInfo *)VBEINFO_ADDR;
  var modes = (u16 *)rmfarptr2ptr(vbe->videoModes);
  var info  = (VESAModeInfo *)VBEMODEINFO_ADDR;
  vbe_get_controller_info(vbe);
  for (size_t i = 0; modes[i] != U16_MAX; i++) {
    int ret = vbe_get_mode_info(modes[i], info);
    if (ret != 0x004f) fatal("Error when vbe_get_mode_info");
    klogd("Mode: %04x %4d x %4d x %4d", modes[i], info->width, info->height, info->bitsPerPixel);
  }
}

int vbe_match_mode(int width, int height, int bpp) {
  var vbe   = (VESAControllerInfo *)VBEINFO_ADDR;
  var modes = (u16 *)rmfarptr2ptr(vbe->videoModes);
  var info  = (VESAModeInfo *)VBEMODEINFO_ADDR;
  vbe_get_controller_info(vbe);
  for (size_t i = 0; modes[i] != U16_MAX; i++) {
    int ret = vbe_get_mode_info(modes[i], info);
    if (ret != 0x004f) fatal("Error when vbe_get_mode_info");
    klogd("%04x:%dx%dx%d", modes[i], info->width, info->height, info->bitsPerPixel);
    if (info->width == width && info->height == height && info->bitsPerPixel == bpp) {
      return modes[i];
    }
  }
  return -1;
}

void *vbe_match_and_set_mode(int width, int height, int bpp) {
  int mode = vbe_match_mode(width, height, bpp);
  if (mode < 0) return null;
  return vbe_set_mode(mode);
}

int vbe_set_buffer(int xoff, int yoff) {
  now_xoff = xoff, now_yoff = yoff;
  regs16 r = {.ax = 0x4f07, .bx = 0x0000, .cx = xoff, .dx = yoff};
  asm16_int(0x10, &r);
  return r.ax;
}

int vbe_flip() {
  void *temp      = vbe_frontbuffer;
  vbe_frontbuffer = vbe_backbuffer;
  vbe_backbuffer  = temp;
  return vbe_set_buffer(now_xoff, now_yoff ? 0 : now_height);
}
