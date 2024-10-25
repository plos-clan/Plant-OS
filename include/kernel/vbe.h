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

/**
 *\brief 初始化VBE
 */
void vbe_init();

/**
 *\brief 获取VBE控制器信息
 *
 *\param addr 指向存储控制器信息的结构体的指针
 *\return 成功返回0，失败返回非0值
 */
int vbe_get_controller_info(VESAControllerInfo *addr);

/**
 *\brief 获取VBE模式信息
 *
 *\param mode VBE模式编号
 *\param addr 指向存储模式信息的结构体的指针
 *\return 成功返回0，失败返回非0值
 */
int vbe_get_mode_info(u16 mode, VESAModeInfo *addr);

/**
 *\brief 打印所有可用的VBE模式
 */
void vbe_print_all_modes();

/**
 *\brief 匹配指定分辨率和颜色深度的VBE模式
 *
 *\param width  宽度
 *\param height 高度
 *\param bpp    每像素位数
 *\return 成功返回模式编号，失败返回-1
 */
int vbe_match_mode(int width, int height, int bpp);

/**
 *\brief 匹配并设置指定分辨率和颜色深度的VBE模式
 *
 *\param width  宽度
 *\param height 高度
 *\param bpp    每像素位数
 *\return 返回指向设置模式后的帧缓冲区的指针，如果失败则返回NULL
 */
void *vbe_match_and_set_mode(int width, int height, int bpp);

/**
 *\brief 设置帧缓冲区偏移
 *
 *\param xoff X轴偏移
 *\param yoff Y轴偏移
 *\return 成功返回0，失败返回非0值
 */
int vbe_set_buffer(int xoff, int yoff);

/**
 *\brief 翻转帧缓冲区
 *
 *\return 成功返回0，失败返回非0值
 */
int vbe_flip();

/**
 *\brief 刷新帧缓冲区
 *
 *\param buf 指向要刷新的缓冲区的指针
 *\return 成功返回0，失败返回非0值
 */
int vbe_flush(const void *buf);

/**
 *\brief 清除屏幕
 *
 *\param r 红色分量
 *\param g 绿色分量
 *\param b 蓝色分量
 *\return 成功返回0，失败返回非0值
 */
int vbe_clear(byte r, byte g, byte b);

static const int screen_w = 1024, screen_h = 768;

extern void *vbe_frontbuffer;
extern void *vbe_backbuffer;
