#pragma once
#include <libc-base.h>

typedef struct {
  u32  setWindow;
  u32  setDisplayStart;
  u32  setPalette;
  u32  IOPrivlnfo;
  long extensionSig;
  long setWindowLen;
  // 展信息标志，恒为 0FBADFBADh
  // 以下信息于缓冲区
  long setDisplayStartLen;
  long setPaletteLen;
  // 以下是代码和表格
} VBEProtectedModeInfo;

typedef struct {
  u16 offset;
  u16 seg;
} ReadModeFarPointer;

typedef struct __PACKED__ {
  u16          attributes;
  u8           winA, winB;
  u16          granularity;
  u16          winsize;
  u16          segmentA, segmentB;
  /* In VBE Specification, this field should be
   * ReadModeFarPointer winPosFunc;
   * However, we overwrite this field in loader n*/
  u16          mode;
  u16          reserved2;
  u16          bytesPerLine;
  volatile u16 width, height;
  volatile u8  Wchar, Ychar, planes, bitsPerPixel, banks;
  u8           memory_model, bank_size, image_pages;
  u8           reserved0;
  u8           red_mask, red_position;
  u8           green_mask, green_position;
  u8           blue_mask, blue_position;
  u8           rsv_mask, rsv_position;
  u8           directcolor_attributes;
  u32          physbase; // your LFB (Linear Framebuffer) address ;)
  u32          offscreen;
  u16          offsize;
} VESAModeInfo;

typedef struct __PACKED__ {
  u8                 signature[4];
  u16                Version;
  ReadModeFarPointer oemString;
  u32                capabilities;
  ReadModeFarPointer videoModes;
  u16                totalMemory;
  u16                OEMVersion;
  ReadModeFarPointer vendor;
  ReadModeFarPointer product;
  ReadModeFarPointer revision;
  /* In VBE Specification, this field should be reserved.
   * However, we overwrite this field in loader */
  u16                modeCount;
  u8                 reserved0[220];
  u8                 oemUse[256];
  VESAModeInfo       modeList[];
} VESAControllerInfo;

#define VBEINFO_ADDR     0x7e00
#define VBEMODEINFO_ADDR 0x7000

void  vbe_init();
int   vbe_get_controller_info(VESAControllerInfo *addr);
int   vbe_get_mode_info(u16 mode, VESAModeInfo *addr);
void *vbe_set_mode(u16 mode);
void  vbe_print_all_modes();
int   vbe_match_mode(int width, int height, int bpp);
void *vbe_match_and_set_mode(int width, int height, int bpp);
int   vbe_set_buffer(int xoff, int yoff);
int   vbe_flip();
int   vbe_flush(const void *buf);
int   vbe_clear(byte r, byte g, byte b);

static const int screen_w = 1024, screen_h = 768;

extern void *vbe_frontbuffer;
extern void *vbe_backbuffer;
