#include "libc-base/asm/asm.h"
#include <kernel.h>

#pragma GCC optimize("O0")

#pragma clang optimize off

#define rmfarptr2ptr(x) ((void *)((x).seg * 0x10 + (x).offset))

int check_vbe_mode(int mode, struct VBEINFO *vinfo) {
  regs16 regs;
  regs.ax = 0x4f01;
  regs.cx = mode + 0x4000;
  regs.es = (int)(vinfo) / 0x10;
  regs.di = (int)(vinfo) % 0x10;
  asm16_int(0x10, &regs);
  if (regs.ax != 0x004f) return -1;
  return 0;
}

int SwitchVBEMode(int mode) {
  struct VBEINFO *vinfo = (struct VBEINFO *)VBEINFO_ADDRESS;
  if (check_vbe_mode(mode, vinfo) != 0) return -1;
  regs16 regs;
  regs.ax = 0x4f02;
  regs.bx = mode + 0x4000;
  asm16_int(0x10, &regs);
  return 0;
}

void SwitchToText8025_BIOS() {
  regs16 regs = {.ax = 0x0003};
  asm16_int(0x10, &regs);
  //   init_palette();
  //   screen_clear();
}

void SwitchTo320X200X256_BIOS() {
  regs16 regs;
  regs.ax = 0x0013;
  asm16_int(0x10, &regs);
}

void *GetSVGACardMemAddress() {
  struct VBEINFO *vinfo = (struct VBEINFO *)VBEINFO_ADDRESS;
  return (void *)(vinfo->vram);
}

char *GetSVGACharOEMString() {
  regs16 regs = {.ax = 0x4f00, .es = 0x07e0, .di = 0x0000};
  asm16_int(0x10, &regs);

  VESAControllerInfo *info = (VESAControllerInfo *)VBEINFO_ADDRESS;
  return rmfarptr2ptr(info->oemString);
}

VESAModeInfo *GetVESAModeInfo(volatile int mode) {
  regs16 r;
  r.ax = 0x4f01;
  r.cx = mode + 0x4000;
  r.es = 0x0700;
  r.di = 0x0000;
  asm16_int(0x10, &r);
  if (r.ax != 0x004f) return NULL;
  return (VESAModeInfo *)(0x7000);
}

// 获取所有支持的模式
// 在屏幕上打印，模式号，分辨率，色深
void get_all_mode() {
  regs16 regs = {.ax = 0x4f00, .es = 0x07e0, .di = 0x0000};
  asm16_int(0x10, &regs);

  auto vbe  = (VESAControllerInfo *)VBEINFO_ADDRESS;
  u16 *mode = (u16 *)rmfarptr2ptr(vbe->videoModes);
  for (size_t c = 0; mode[c] != U16_MAX; c++) {
    VESAModeInfo *info = GetVESAModeInfo(mode[c]);
    klogd("Mode: %04x %4d x %4d x %4d", mode[c], info->width, info->height, info->bitsPerPixel);
  }
}

u32 set_mode(int width, int height, int bpp) {
  regs16 regs = {.ax = 0x4f00, .es = 0x07e0, .di = 0x0000};
  asm16_int(0x10, &regs);

  auto          vbe  = (VESAControllerInfo *)VBEINFO_ADDRESS;
  volatile u16 *mode = (u16 *)rmfarptr2ptr(vbe->videoModes);
  for (size_t c = 0; mode[c] != U16_MAX; c++) {
    volatile VESAModeInfo *info = GetVESAModeInfo(mode[c]);
    // klogd("%04x:%dx%dx%d", mode[c], info->width, info->height, info->bitsPerPixel);
    if (info->width == width && info->height == height && info->bitsPerPixel == bpp) {
      SwitchVBEMode(mode[c]);
      volatile struct VBEINFO *v = (struct VBEINFO *)VBEINFO_ADDRESS;
      return v->vram;
    }
  }
  return -1;
}
