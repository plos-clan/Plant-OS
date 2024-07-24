#pragma once
#include <libc.h>
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
  VESAModeInfo       modeList[0];
} VESAControllerInfo;
struct VBEINFO {
  char res1[18];
  i16  xsize, ysize;
  char res2[18];
  int  vram;
};
#define VBEINFO_ADDRESS 0x7e00

u32 set_mode(int width, int height, int bpp);