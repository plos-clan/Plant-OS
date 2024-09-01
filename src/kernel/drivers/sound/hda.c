// 此驱动可能注释不全，请参考https://server.plos-clan.org/hda-programming-document.html
#pragma clang optimize off
#include <kernel.h>
#include <mmio.h>
#define SEND_VERB_METHOD_DMA 1
#define SEND_VERB_METHOD_MMIO 2

static u8 hda_bus = 255, hda_slot = 255, hda_func = 255;
static u32 hda_base, output_base;
static u32 *corb = null;
static u32 *rirb = null;
static u32 corb_entry_count = 0, rirb_entry_count = 0;
static u32 corb_write_pointer = 0, rirb_read_pointer = 0;
static u32 send_verb_method;
static void wait(int ticks) {
  int tick = timerctl.count;

  while (timerctl.count - tick < ticks)
    ;
}
u8 minb(u32 addr)
{
    return *((volatile u8 *)addr);
}

u16 minw(u32 addr)
{
    return *((volatile u16 *)addr);
}

u32 minl(u32 addr)
{
    return *((volatile u32 *)addr);
}

void moutb(u32 addr, u8 value)
{
    *((volatile u8 *)addr) = value;
}

void moutw(u32 addr, u16 value)
{
    *((volatile u16 *)addr) = value;
}

void moutl(u32 addr, u32 value)
{
    *((volatile u32 *)addr) = value;
}
u32 hda_verb(u32 codec, u32 node, u32 verb, u32 command) {
  if (verb == 0x2 || verb == 0x3) {
    verb <<= 8;
  }
  u32 value = ((codec << 28) | (node << 20) | (verb << 8) | (command));
  if (send_verb_method == SEND_VERB_METHOD_DMA) {
    corb[corb_write_pointer] = value;
    moutw(hda_base + 0x48, corb_write_pointer);
    u32 ticks = timerctl.count;
    while (timerctl.count - ticks < 1) {
      if ((minw(hda_base + 0x58)) == corb_write_pointer) {
        break;
      }
    }
    if ((minw(hda_base + 0x58) & 0xff) != corb_write_pointer) {
      error("no response from hda");
      return 0;
    }
    value = rirb[corb_write_pointer * 2];
    corb_write_pointer = (corb_write_pointer + 1) % corb_entry_count;
    rirb_read_pointer = (rirb_read_pointer + 1) % rirb_entry_count;
    return value;
  } else {
    moutw(hda_base + 0x68, 0b10);
    moutl(hda_base + 0x60, value);
    moutw(hda_base + 0x68, 1);
    u32 ticks = timerctl.count;
    while (timerctl.count - ticks < 1) {
      if ((minw(hda_base + 0x68) & 0x3) == 0b10) {
        break;
      }
    }
    if ((minw(hda_base + 0x68) & 0x3) != 0b10) {
      moutw(hda_base + 0x68, 0b10);
      error("no response from hda");
      return 0;
    } else {
      moutw(hda_base + 0x68, 0b10);
    }
    return minl(hda_base + 0x64);
  }
}

void hda_init() {
  klogd("hda_init");
  // 许多 HDA 设备的 Vendor ID 为 8086（Intel），Device ID 为 2668 或
  // 27D8，但其他 Vendor ID 也在使用中，例如 Vendor ID 1002（AMD）和 Device ID
  // 4383。
  pci_get_device(0x8086, 0x2668, &hda_bus, &hda_slot, &hda_func);
  if (hda_bus == 255 || hda_slot == 255 || hda_func == 255) {
    pci_get_device(0x8086, 0x27D8, &hda_bus, &hda_slot, &hda_func);
    if (hda_bus == 255 || hda_slot == 255 || hda_func == 255) {
      pci_get_device(0x1002, 0x4383, &hda_bus, &hda_slot, &hda_func);
    }
  }
  if (hda_bus == 255 || hda_slot == 255 || hda_func == 255) {
    error("hda card not found");
    return;
  }
   write_pci(hda_bus, hda_slot, hda_func, 0x04, ((read_pci(hda_bus, hda_slot, hda_func, 0x04) & ~(1<<10)) | (1<<2) | (1<<1))); //enable interrupts, enable bus mastering, enable MMIO space
  info("hda card found at bus %d slot %d func %d", hda_bus, hda_slot, hda_func);
  hda_base = read_bar_n(hda_bus, hda_slot, hda_func, 0);
  info("hda base address: 0x%x", hda_base);
  moutl(hda_base + 0x08, 0);
  int ticks = timerctl.count;
  while (timerctl.count - ticks < 1) {
    if ((minl(hda_base + 0x08) & 0x01) == 0) {
      break;
    }
  }
  if ((minl(hda_base + 0x08) & 0x01) != 0) {
    error("hda reset failed");
    return;
  }
  moutl(hda_base + 0x08, 1);
  ticks = timerctl.count;
  while (timerctl.count - ticks < 1) {
    if ((minl(hda_base + 0x08) & 0x01) == 1) {
      break;
    }
  }
  if ((minl(hda_base + 0x08) & 0x01) != 1) {
    error("hda reset failed");
    return;
  }
  info("hda card is working now");
  int input_stream_count = (minw(hda_base + 0x00) >> 8) & 0x0f;
  info("input stream count: %d", input_stream_count);
  output_base = hda_base + 0x80 + (0x20 * input_stream_count);
  info("output base address: 0x%x", output_base);
  moutl(hda_base + 0x20, 0);

  moutl(hda_base + 0x70, 0);
  moutl(hda_base + 0x74, 0);

  moutl(hda_base + 0x34, 0);
  moutl(hda_base + 0x38, 0);

  moutb(hda_base + 0x4c, 0);
  moutb(hda_base + 0x5c, 0);

  corb = page_malloc_one_no_mark();
  moutl(hda_base + 0x40, (u32)corb);
  moutl(hda_base + 0x44, 0);

  u8 corb_size = minb(hda_base + 0x4e) >> 4 & 0x0f;
  if (corb_size & 0b0001) {
    corb_entry_count = 2;
    moutb(hda_base + 0x4e, 0b00);
  } else if (corb_size & 0b0010) {
    corb_entry_count = 16;
    moutb(hda_base + 0x4e, 0b01);
  } else if (corb_size & 0b0100) {
    corb_entry_count = 256;
    moutb(hda_base + 0x4e, 0b10);
  } else {
    error("corb size not supported");
    goto mmio;
  }
  info("corb size: %d entries", corb_entry_count);

  moutw(hda_base + 0x4A, 0x8000);
  ticks = timerctl.count;
  while (timerctl.count - ticks < 1) {
    
    if ((minw(hda_base + 0x4A) & 0x8000)) {
      break;
    }
  }
  if ((minw(hda_base + 0x4A) & 0x8000) == 0) {
    error("corb reset failed");
    goto mmio;
  }
  moutw(hda_base + 0x4A, 0x0000);
  ticks = timerctl.count;
  while (timerctl.count - ticks < 1) {
    if ((minw(hda_base + 0x4A) & 0x8000) == 0) {
      break;
    }
  }
  if ((minw(hda_base + 0x4A) & 0x8000) != 0) {
    error("corb reset failed");
    goto mmio;
  }
  moutw(hda_base + 0x48, 0);

  corb_write_pointer = 1;
  info("corb has been reset already");

  rirb = page_malloc_one_no_mark();
  moutl(hda_base + 0x50, (u32)rirb);
  moutl(hda_base + 0x54, 0);

  u8 rirb_size = minb(hda_base + 0x5e) >> 4 & 0x0f;
  if (rirb_size & 0b0001) {
    rirb_entry_count = 2;
    moutb(hda_base + 0x5e, 0b00);
  } else if (rirb_size & 0b0010) {
    rirb_entry_count = 16;
    moutb(hda_base + 0x5e, 0b01);
  } else if (rirb_size & 0b0100) {
    rirb_entry_count = 256;
    moutb(hda_base + 0x5e, 0b10);
  } else {
    error("rirb size not supported");
    goto mmio;
  }
  info("rirb size: %d entries", rirb_entry_count);

  moutw(hda_base + 0x58, 0x8000);
  wait(1);

  moutw(hda_base + 0x5A, 0x0000);

  rirb_read_pointer = 1;
  info("rirb has been reset already");

  moutb(hda_base + 0x4c, 0b10);
  moutb(hda_base + 0x5c, 0b10);
  info("corb rirb dma has been started");
  send_verb_method = SEND_VERB_METHOD_DMA;
  for (int i = 0; i < 16; i++) {
    u32 codec_id = hda_verb(i, 0, 0xf00, 0);
    if (codec_id != 0) {
      printf("found codec %d %x\n", i, codec_id);
      return;
    }
  }
mmio:
  warn("use mmio");
  send_verb_method = SEND_VERB_METHOD_MMIO;
  moutb(output_base + 0x4c, 0);
  moutb(output_base + 0x5c, 0);
  for (int i = 0; i < 16; i++) {
    u32 codec_id = hda_verb(i, 0, 0xf00, 0);
    if (codec_id != 0) {
      printf("found codec %d %x\n", i, codec_id);
      return;
    }
  }
}