#include <kernel.h>
// https://github.com/StevenBaby/onix/blob/dev/src/kernel/sb16.c

void asm_sb16_handler(int *esp);

struct __PACKED__ WAV16_HEADER {
  char riff[4];
  int  size;
  char wave[4];
  char fmt[4];
  int  fmt_size;
  i16  format;
  i16  channel;
  int  sample_rate;
  int  byte_per_sec;
};

#define SB_MIXER      0x224 // DSP 混合器端口
#define SB_MIXER_DATA 0x225 // DSP 混合器数据端口
#define SB_RESET      0x226 // DSP 重置
#define SB_READ       0x22A // DSP 读
#define SB_WRITE      0x22C // DSP 写
#define SB_STATE      0x22E // DSP 读状态
#define SB_INTR16     0x22F // DSP 16 位中断响应

#define CMD_STC  0x40 // Set Time Constant
#define CMD_SOSR 0x41 // Set Output Sample Rate
#define CMD_SISR 0x42 // Set Input Sample Rate

#define CMD_SINGLE_IN8   0xC8 // Transfer mode 8bit input
#define CMD_SINGLE_OUT8  0xC0 // Transfer mode 8bit output
#define CMD_SINGLE_IN16  0xB8 // Transfer mode 16bit input
#define CMD_SINGLE_OUT16 0xB0 // Transfer mode 16bit output

#define CMD_AUTO_IN8   0xCE // Transfer mode 8bit input auto
#define CMD_AUTO_OUT8  0xC6 // Transfer mode 8bit output auto
#define CMD_AUTO_IN16  0xBE // Transfer mode 16bit input auto
#define CMD_AUTO_OUT16 0xB6 // Transfer mode 16bit output auto

#define CMD_ON      0xD1 // Turn speaker on
#define CMD_OFF     0xD3 // Turn speaker off
#define CMD_SP8     0xD0 // Stop playing 8 bit channel
#define CMD_RP8     0xD4 // Resume playback of 8 bit channel
#define CMD_SP16    0xD5 // Stop playing 16 bit channel
#define CMD_RP16    0xD6 // Resume playback of 16 bit channel
#define CMD_VERSION 0xE1 // Turn speaker off

#define MODE_MONO8    0x00
#define MODE_STEREO8  0x20
#define MODE_MONO16   0x10
#define MODE_STEREO16 0x30

#define STATUS_READ  0x80 // read buffer status
#define STATUS_WRITE 0x80 // write buffer status

#define DMA_BUF_SIZE 0x8000 // 缓冲区长度

static void *const DMA_BUF_ADDR1 = (void *)0x90000;                // 不能跨越 64K 边界
static void *const DMA_BUF_ADDR2 = (void *)0x90000 + DMA_BUF_SIZE; // 不能跨越 64K 边界

#define SAMPLE_RATE 44100 // 采样率
#define SB16_IRQ    5

struct sb16 {
  mtask          *use_task;
  int             status;
  char           *addr1;   // DMA 地址
  volatile size_t size1;   //
  char           *addr2;   // DMA 地址
  volatile size_t size2;   //
  u8              mode;    // 模式
  u8              channel; // DMA 通道
};

struct sound_settings {
  u32 sample_rate; //
  f32 volume;      //
};

static struct sb16 sb;

void sb_exch_dmaaddr() {
  char *addr  = sb.addr1;
  sb.addr1    = sb.addr2;
  sb.addr2    = addr;
  size_t size = sb.size1;
  sb.size1    = sb.size2;
  sb.size2    = size;
}

static void sb_out(u8 cmd) {
  while (asm_in8(SB_WRITE) & 128) {}
  asm_out8(SB_WRITE, cmd);
}

void sb16_do_dma() {
  // 设置采样率
  sb_out(CMD_SOSR);                  // 44100 = 0xAC44
  sb_out((SAMPLE_RATE >> 8) & 0xFF); // 0xAC
  sb_out(SAMPLE_RATE & 0xFF);        // 0x44

  dma_send(sb.channel, (u32)(sb.addr2), sb.size2, 0);
  if (sb.mode == MODE_MONO8) {
    sb_out(CMD_SINGLE_OUT8);
    sb_out(MODE_MONO8);
  } else {
    sb_out(CMD_SINGLE_OUT16);
    sb_out(MODE_STEREO16);
  }

  sb_out((sb.size2 - 1) & 0xFF);
  sb_out(((sb.size2 - 1) >> 8) & 0xFF);
}

void sb16_do_close() {
  sb_out(CMD_OFF); // 关闭声卡
  sb.use_task = NULL;
}

void sb16_handler(int *esp) {
  send_eoi(SB16_IRQ);

  asm_in8(SB_INTR16);
  u8 state = asm_in8(SB_STATE);

  asm_cli;
  sb.size2 = 0;
  if (sb.size1 > 0) {
    sb_exch_dmaaddr();
    sb16_do_dma();
  }
  asm_cli;

  if (sb.status > 0) {
    sb16_do_close();
    return;
  }

  task_run(sb.use_task);
}

void sb16_init() {
  sb.addr1    = DMA_BUF_ADDR1;
  sb.addr2    = DMA_BUF_ADDR2;
  sb.mode     = MODE_STEREO16;
  sb.channel  = 5;
  sb.use_task = NULL;
  sb.size1    = 0;
  sb.size2    = 0;
  sb.status   = 0;
  irq_mask_clear(SB16_IRQ);
  register_intr_handler(SB16_IRQ + 0x20, (u32)asm_sb16_handler);
}

static void sb_reset() {
  asm_out8(SB_RESET, 1);
  auto oldcnt = timerctl.count;
  while (timerctl.count <= oldcnt + 10) {}
  asm_out8(SB_RESET, 0);
  u8 state = asm_in8(SB_READ);
  klogd("sb16 reset state 0x%x", state);
}

static void sb_intr_irq() {
  asm_out8(SB_MIXER, 0x80);
  u8 data = asm_in8(SB_MIXER_DATA);
  if (data != 2) {
    asm_out8(SB_MIXER, 0x80);
    asm_out8(SB_MIXER_DATA, 0x2);
  }
}

void sb16_set_volume(u8 level) {
  klogd("set sb16 volume to %d/255", level);
  asm_out8(SB_MIXER, 0x22);
  asm_out8(SB_MIXER_DATA, level);
}

void sb16_open() {
  while (sb.use_task) {}
  asm_cli;
  sb.use_task = current_task();
  asm_sti;
  sb_reset();     // 重置 DSP
  sb_intr_irq();  // 设置中断
  sb_out(CMD_ON); // 打开声卡

  sb.mode    = MODE_MONO8;
  sb.channel = 1;
  sb.size1   = 0;
  sb.size2   = 0;
  sb.status  = 0;

  // sb.mode    = MODE_STEREO16;
  // sb.channel = 5;
}

void sb16_close() {
  sb.status = 1;
  if (sb.size2 > 0) return;
  sb16_do_close();
}

int sb16_write(void *data, size_t size) {
  while (sb.size1) {}

  memcpy(sb.addr1, data, size);
  sb.size1 = size;

  if (sb.size2 > 0) return size;

  sb_exch_dmaaddr();
  sb16_do_dma();

  return size;
}

// 未完成

typedef struct vsound *vsound_t;

typedef int (*vsound_open_t)(vsound_t vsound);
typedef int (*vsound_close_t)(vsound_t vsound);
typedef ssize_t (*vsound_read_t)(vsound_t vsound, void *addr, size_t size);
typedef ssize_t (*vsound_write_t)(vsound_t vsound, const void *addr, size_t size);
typedef ssize_t (*vsound_callback_t)(vsound_t vsound, const void *addr, size_t size);

typedef struct vsound {
  cstr              name;     //
  vsound_open_t     open;     //
  vsound_close_t    close;    //
  vsound_read_t     read;     //
  vsound_write_t    write;    //
  vsound_callback_t played;   // 音频播放完毕(前)，请求下一段音频
  void             *buf;      // 音频缓冲区
  size_t            bufsize;  //
  void             *userdata; // 可自定义
} *vsound_t;

// struct vsound vsound = {
//     .name  = "sb16",
//     .open  = sb16_open,
//     .close = sb16_close,
//     .write = sb16_write,
// };

rbtree_sp_t vsound_list;

bool vsound_regist(vsound_t device) {
  if (device == null) return false;
  if (rbtree_sp_get(vsound_list, device->name)) return false;
  rbtree_sp_insert(vsound_list, device->name, device);
  return true;
}

bool vsound_open() {
  return true;
}

// int sound_open(cstr device) {
//   if (device == null) return open("/dev/sound/default");
// }
