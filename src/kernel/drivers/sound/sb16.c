#include <kernel.h>
#include <sound.h>

void asm_sb16_handler(int *esp);

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

#define MODE_SMONO   (0b01 << 4)
#define MODE_SSTEREO (0b11 << 4)
#define MODE_UMONO   (0b00 << 4)
#define MODE_USTEREO (0b10 << 4)

#define STATUS_READ  0x80 // read buffer status
#define STATUS_WRITE 0x80 // write buffer status

#define DMA_BUF_SIZE 0x8000 // 缓冲区长度

static void *const DMA_BUF_ADDR1 = (void *)0x90000;                // 不能跨越 64K 边界
static void *const DMA_BUF_ADDR2 = (void *)0x90000 + DMA_BUF_SIZE; // 不能跨越 64K 边界

#define SB16_IRQ 5

static int sample_rate = 44100;

struct sb16 {
  mtask          *use_task; //
  int             status;   //
  char           *addr1;    // DMA 地址
  volatile size_t size1;    //
  char           *addr2;    // DMA 地址
  volatile size_t size2;    //
  u8              mode;     // 模式
  u8              channel;  // DMA 通道
  byte            depth;    // 采样深度
};

struct sound_settings {
  u32 sample_rate; //
  f32 volume;      //
};

static struct sb16 sb;

static void sb_exch_dmaaddr() {
  char *addr  = sb.addr1;
  sb.addr1    = sb.addr2;
  sb.addr2    = addr;
  size_t size = sb.size1;
  sb.size1    = sb.size2;
  sb.size2    = size;
}

static void sb_send(u8 cmd) {
  waitif(asm_in8(SB_WRITE) & MASK(7));
  asm_out8(SB_WRITE, cmd);
}

static void sb16_do_dma() {
  // 设置采样率
  sb_send(CMD_SOSR);
  sb_send((sample_rate >> 8) & 0xff);
  sb_send(sample_rate & 0xff);

  dma_send(sb.channel, (u32)(sb.addr2), sb.size2, 0, sb.depth == 16);
  sb_send(sb.depth == 8 ? CMD_SINGLE_OUT8 : CMD_SINGLE_OUT16);
  sb_send(sb.mode);

  size_t len = sb.depth == 8 ? sb.size2 : sb.size2 / 2;
  sb_send((len - 1) & 0xff);
  sb_send(((len - 1) >> 8) & 0xff);
}

void sb16_do_close() {
  sb_send(CMD_OFF); // 关闭声卡
  sb.use_task = null;
}

void sb16_handler(int *esp) {
  send_eoi(SB16_IRQ);

  asm_in8(sb.depth == 16 ? SB_INTR16 : SB_STATE);

  sb.size2 = 0;
  if (sb.size1 > 0) {
    sb_exch_dmaaddr();
    sb16_do_dma();
  }

  asm_out8(0x20, 0x20);

  if (sb.status > 0) {
    sb16_do_close();
    return;
  }

  task_run(sb.use_task);
}

void sb16_init() {
  sb.use_task = null;
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

// 支持
//  S8 U8 S16 U16
int sb16_open(int rate, int channels, sound_pcmfmt_t fmt) {
  if (rate <= 0 || rate > 192000) return -1;
  if (channels <= 0 || channels > 2) return -1;

  asm_cli;
  if (sb.use_task != null) {
    asm_sti;
    return -1;
  }
  sb.use_task = current_task();
  asm_sti;

  sample_rate = rate;
  sb_reset();      // 重置 DSP
  sb_intr_irq();   // 设置中断
  sb_send(CMD_ON); // 打开声卡

  if (fmt == SOUND_FMT_S16 || fmt == SOUND_FMT_U16) {
    sb.depth   = 16;
    sb.channel = 5;
  } else if (fmt == SOUND_FMT_U8 || fmt == SOUND_FMT_S8) {
    sb.depth   = 8;
    sb.channel = 1;
  }
  if (channels == 1) {
    sb.mode = (fmt == SOUND_FMT_S16 || fmt == SOUND_FMT_S8) ? MODE_SMONO : MODE_UMONO;
  } else {
    sb.mode = (fmt == SOUND_FMT_S16 || fmt == SOUND_FMT_S8) ? MODE_SSTEREO : MODE_USTEREO;
  }
  sb.addr1  = DMA_BUF_ADDR1;
  sb.addr2  = DMA_BUF_ADDR2;
  sb.size1  = 0;
  sb.size2  = 0;
  sb.status = 0;

  return 0;
}

void sb16_close() {
  if (sb.use_task != current_task()) return;
  sb.status = 1;
  if (sb.size2 > 0) return;
  sb16_do_close();
}

int sb16_write(void *data, size_t size) {
  waitif(sb.size1 > 0);

  memcpy(sb.addr1, data, size);
  sb.size1 = size;

  if (sb.size2 > 0) return size;

  sb_exch_dmaaddr();
  sb16_do_dma();

  return size;
}

// struct vsound vsound = {
//     .name  = "sb16",
//     .open  = sb16_open,
//     .close = sb16_close,
//     .write = sb16_write,
// };
