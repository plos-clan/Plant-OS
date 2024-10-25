#include <kernel.h>

#define rmfarptr2ptr(x) ((void *)((x).seg * 0x10 + (x).offset))

static int now_width = 0, now_height = 0;
static int now_xoff = 0, now_yoff = 0;

void *vbe_frontbuffer;
void *vbe_backbuffer;

void vbe_set_mode_8025() {
  regs16 regs = {.ax = 0x0003};
  v86_int(0x10, &regs);
}

int vbe_get_controller_info(VESAControllerInfo *addr) {
  regs16 r = {.ax = 0x4f00, .es = (size_t)addr / 16, .di = (size_t)addr % 16};
  v86_int(0x10, &r);
  if (r.ax != 0x004f) kloge("Error when vbe_get_controller_info, rets %04x", r.ax);
  klogd("你有 %dMiB %dKiB 的显存", addr->totalMemory * 64 / 1024, addr->totalMemory * 64 % 1024);
  return r.ax;
}

char *GetSVGACharOEMString() {
  var vbe = (VESAControllerInfo *)VBEINFO_ADDR;
  vbe_get_controller_info(vbe);
  return rmfarptr2ptr(vbe->oemString);
}

int vbe_get_mode_info(u16 mode, VESAModeInfo *addr) {
  mode     |= MASK(14);
  regs16 r  = {.ax = 0x4f01, .cx = mode, .es = (size_t)addr / 16, .di = (size_t)addr % 16};
  v86_int(0x10, &r);
  if (r.ax != 0x004f) kloge("Error when vbe_get_mode_info, mode %04x rets %04x", mode, r.ax);
  return r.ax;
}

void *vbe_set_mode(u16 mode) {
  mode        |= MASK(14);
  var    addr  = (VESAModeInfo *)VBEMODEINFO_ADDR;
  regs16 r1    = {.ax = 0x4f01, .cx = mode, .es = (size_t)addr / 16, .di = (size_t)addr % 16};
  v86_int(0x10, &r1);
  assert(r1.ax == 0x004f, "Error when vbe_set_mode");

  regs16 r2 = {.ax = 0x4f02, .bx = mode};
  v86_int(0x10, &r2);
  assert(r2.ax == 0x004f, "Error when vbe_set_mode");

  size_t size = addr->width * addr->height * addr->bitsPerPixel / 8;
  klogd("设置 VBE 模式成功，该模式需要 %dMiB %dKiB 显存", size / 1024 / 1024, size / 1024 % 1024);

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
  int ret   = vbe_get_controller_info(vbe);
  assert(ret == 0x004f, "vbe_get_controller_info failed.");
  for (size_t i = 0; modes[i] != U16_MAX; i++) {
    int ret = vbe_get_mode_info(modes[i], info);
    if (ret != 0x004f) continue;
    klogd("Mode: %04x %4d x %4d x %4d", modes[i], info->width, info->height, info->bitsPerPixel);
  }
}

int vbe_match_mode(int width, int height, int bpp) {
  var vbe   = (VESAControllerInfo *)VBEINFO_ADDR;
  var modes = (u16 *)rmfarptr2ptr(vbe->videoModes);
  var info  = (VESAModeInfo *)VBEMODEINFO_ADDR;
  int ret   = vbe_get_controller_info(vbe);
  assert(ret == 0x004f, "vbe_get_controller_info failed.");
  for (size_t i = 0; modes[i] != U16_MAX; i++) {
    int ret = vbe_get_mode_info(modes[i], info);
    if (ret != 0x004f) continue;
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
  v86_int(0x10, &r);
  if (r.ax != 0x004f) kloge("Error when vbe_set_buffer");
  return r.ax == 0x004f ? 0 : -1;
}

int vbe_flip() {
  void *temp      = vbe_frontbuffer;
  vbe_frontbuffer = vbe_backbuffer;
  vbe_backbuffer  = temp;
  return vbe_set_buffer(now_xoff, now_yoff ? 0 : now_height);
}

int vbe_flush(const void *buf) {
  if (vbe_backbuffer == null) return -1;
  lgmemcpy32(vbe_backbuffer, buf, now_width * now_height);
  return vbe_flip();
}

int vbe_clear(byte r, byte g, byte b) {
  lgmemset32(vbe_backbuffer, r << 16 | g << 8 | b, now_width * now_height);
  return vbe_flip();
}

typedef union PMInfoBlock {
  struct {
    u32  signature;       // PM Info Block Signature
    u16  entry_point;     // Offset of PM entry point within BIOS
    u16  pm_initialize;   //  Offset of PM initialization entry point
    u16  bios_data_sel;   // Selector to BIOS data area emulation block
    u16  A0000_sel;       // Selector to access A0000h physical mem
    u16  B0000_sel;       // Selector to access B0000h physical mem
    u16  B8000_sel;       // Selector to access B8000h physical mem
    u16  code_seg_sel;    // Selector to access code segment as data
    bool in_protect_mode; // Set to 1 when in protected mode
    byte checksum;        // Checksum byte for structure
  };
  byte data[20];
} PMInfoBlock;

#define BIOS_ADDR ((void *)0xc0000)
#define BIOS_SIZE ((size_t)0x8000 * 4)

static void vbe_find_pminfo() {
  void *bios_copy = malloc(BIOS_SIZE);
  memcpy(bios_copy, BIOS_ADDR, BIOS_SIZE);
  PMInfoBlock *info = null;
  for (size_t i = 0; i < BIOS_SIZE - sizeof(PMInfoBlock); i++) {
    PMInfoBlock *blk = BIOS_ADDR + i;
    if (blk->signature != MAGIC32('P', 'M', 'I', 'D')) continue;
    klogd("Found PMInfoBlock signature at %p", blk);
    byte sum = 0;
#pragma unroll
    for (size_t i = 0; i < sizeof(*blk); i++) {
      sum += blk->data[i];
    }
    if (sum != 0) continue;
    info = blk;
    break;
  }
  if (info == null) {
    klogw("No PMInfoBlock found in BIOS");
    return;
  }
}

void vbe_init() {}
