#include "kernel/log.h"
#include <kernel.h>
#define Phy(seg, off)   (seg * 0x10 + off)
#define rmfarptr2ptr(x) ((void *)((x).seg * 0x10 + (x).offset))
int check_vbe_mode(int mode, struct VBEINFO *vinfo) {
  regs16_t regs;
  regs.ax = 0x4f01;
  regs.cx = mode + 0x4000;
  regs.es = (int)(vinfo) / 0x10;
  regs.di = (int)(vinfo) % 0x10;
  INT(0x10, &regs);
  if (regs.ax != 0x004f) return -1;
  return 0;
}
int SwitchVBEMode(int mode) {
  struct VBEINFO *vinfo = (struct VBEINFO *)VBEINFO_ADDRESS;
  if (check_vbe_mode(mode, vinfo) != 0) return -1;
  regs16_t regs;
  regs.ax = 0x4f02;
  regs.bx = mode + 0x4000;
  INT(0x10, &regs);
  return 0;
}
void SwitchToText8025_BIOS() {
  regs16_t regs;
  regs.ax = 0x0003;
  INT(0x10, &regs);
  //   init_palette();
  //   clear();
}
void SwitchTo320X200X256_BIOS() {
  regs16_t regs;
  regs.ax = 0x0013;
  INT(0x10, &regs);
}
void *GetSVGACardMemAddress() {
  struct VBEINFO *vinfo = (struct VBEINFO *)VBEINFO_ADDRESS;
  return (void *)(vinfo->vram);
}
char *GetSVGACharOEMString() {
  struct VBEINFO *vbeinfo = (struct VBEINFO *)VBEINFO_ADDRESS;
  regs16_t        r;
  r.ax = 0x4f00;
  r.es = 0x07e0;
  r.di = 0x0000;
  INT(0x10, &r);
  VESAControllerInfo *info = (VESAControllerInfo *)VBEINFO_ADDRESS;
  return rmfarptr2ptr(info->oemString);
}
VESAModeInfo *GetVESAModeInfo(int mode) {
  regs16_t r;
  r.ax = 0x4f01;
  r.cx = mode + 0x4000;
  r.es = 0x0700;
  r.di = 0x0000;
  INT(0x10, &r);
  if (r.ax != 0x004f) return NULL;
  return (VESAModeInfo *)(0x7000);
}
void _get_all_mode() {
  // 获取所有支持的模式
  // 在屏幕上打印，模式号，分辨率，色深
  regs16_t regs;
  regs.ax = 0x4f00;
  regs.es = 0x07e0;
  regs.di = 0x0000;
  INT(0x10, &regs);
  VESAControllerInfo *vbe  = (struct VESAControllerInfo *)VBEINFO_ADDRESS;
  u16                *mode = (u16 *)rmfarptr2ptr(vbe->videoModes);
  // int i = 0;
  for (int c = 0;; c++) {
    if (mode[c] == 0xffff) break;
    logd("Mode: %04x ", mode[c]);
    VESAModeInfo *info = GetVESAModeInfo(mode[c]);
    logd("%d x %d x %d\n", info->width, info->height, info->bitsPerPixel);
    // sleep(500);
  }
}
void get_all_mode() {
  // 获取所有支持的模式
  // 在屏幕上打印，模式号，分辨率，色深

  _get_all_mode();
  //
}
unsigned set_mode(int width, int height, int bpp) {
  regs16_t regs;
  regs.ax = 0x4f00;
  regs.es = 0x07e0;
  regs.di = 0x0000;
  INT(0x10, &regs);
  VESAControllerInfo *vbe  = (struct VESAControllerInfo *)VBEINFO_ADDRESS;
  u16                *mode = (u16 *)rmfarptr2ptr(vbe->videoModes);
  // int i = 0;
  for (int c = 0;; c++) {
    if (mode[c] == 0xffff) break;
    VESAModeInfo *info = GetVESAModeInfo(mode[c]);
    if (info->width == width && info->height == height && info->bitsPerPixel == bpp) {
      SwitchVBEMode(mode[c]);
      struct VBEINFO *v = (struct VBEINFO *)VBEINFO_ADDRESS;
      return v->vram;
    } else {
      //   printk("%dx%dx%d\n",info->width,info->height,info->bitsPerPixel);
    }
  }
  return -1;
}