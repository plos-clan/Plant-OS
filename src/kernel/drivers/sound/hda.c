// 此驱动可能注释不全，请参考https://server.plos-clan.org/hda-programming-document.html

#include <kernel.h>

#define SEND_VERB_METHOD_DMA      1
#define SEND_VERB_METHOD_MMIO     2
#define HDA_WIDGET_AUDIO_OUTPUT   0x0
#define HDA_WIDGET_AUDIO_INPUT    0x1
#define HDA_WIDGET_MIXER          0x2
#define HDA_WIDGET_AUDIO_SELECTOR 0x3
#define HDA_WIDGET_PIN_COMPLEX    0x4
#define HDA_WIDGET_POWER_WIDGET   0x5
#define HDA_WIDGET_VKW            0x6
#define HDA_WIDGET_BG             0x7
#define HDA_WIDGET_VENDOR         0xF

#define HDA_PIN_COMPLEX_LINE_OUT           0x0
#define HDA_PIN_COMPLEX_SPEAKER            0x1
#define HDA_PIN_COMPLEX_HP_OUT             0x2
#define HDA_PIN_COMPLEX_CD                 0x3
#define HDA_PIN_COMPLEX_SPDIF_OUT          0x4
#define HDA_PIN_COMPLEX_DIG_OUT            0x5
#define HDA_PIN_COMPLEX_MODEM_LINE_SIDE    0x6
#define HDA_PIN_COMPLEX_MODEM_HANDSET_SIDE 0x7
#define HDA_PIN_COMPLEX_LINE_IN            0x8
#define HDA_PIN_COMPLEX_AUX                0x9
#define HDA_PIN_COMPLEX_MIC_IN             0xa
#define HDA_PIN_COMPLEX_TELEPHONY          0xb
#define HDA_PIN_COMPLEX_SPDIF_IN           0xc
#define HDA_PIN_COMPLEX_DIG_IN             0xd

static u8   hda_bus = 255, hda_slot = 255, hda_func = 255;
static u32  hda_base, output_base;
static u32 *corb             = null;
static u32 *rirb             = null;
static u32  corb_entry_count = 0, rirb_entry_count = 0;
static u32  corb_write_pointer = 0, rirb_read_pointer = 0;
static u32  send_verb_method;
static u32  hda_afg_pcm_format_cap    = 0;
static u32  hda_afg_stream_format_cap = 0;
static u32  hda_afg_amp_cap           = 0;

static u32 hda_pin_output_node       = 0;
static u32 hda_pin_output_amp_cap    = 0;
static u32 hda_pin_pcm_format_cap    = 0;
static u32 hda_pin_stream_format_cap = 0;

static u32 hda_pin_output_node_second       = 0;
static u32 hda_pin_output_amp_cap_second    = 0;
static u32 hda_pin_pcm_format_cap_second    = 0;
static u32 hda_pin_stream_format_cap_second = 0;

static u32  *output_buffer    = null;
static u32   hda_codec_number = 0;
static void *hda_buffer_ptr   = null;

static inthandler_f hda_interrupt_handler;
static inthandler_t prev_handler;

static void wait(int ticks) {
  int tick = system_tick;
  waituntil(system_tick - tick < ticks);
}

u32 hda_verb(u32 codec, u32 node, u32 verb, u32 command) {
  if (verb == 0x2 || verb == 0x3) { verb <<= 8; }
  u32 value = ((codec << 28) | (node << 20) | (verb << 8) | (command));
  if (send_verb_method == SEND_VERB_METHOD_DMA) {
    corb[corb_write_pointer] = value;
    mem_set16(hda_base + 0x48, corb_write_pointer);
    u32 ticks = system_tick;
    while (system_tick - ticks < 1) {
      if ((mem_get16(hda_base + 0x58)) == corb_write_pointer) { break; }
    }
    if ((mem_get16(hda_base + 0x58) & 0xff) != corb_write_pointer) {
      error("no response from hda");
      return 0;
    }
    value              = rirb[corb_write_pointer * 2];
    corb_write_pointer = (corb_write_pointer + 1) % corb_entry_count;
    rirb_read_pointer  = (rirb_read_pointer + 1) % rirb_entry_count;
    return value;
  } else {
    mem_set16(hda_base + 0x68, 0b10);
    mem_set32(hda_base + 0x60, value);
    mem_set16(hda_base + 0x68, 1);
    u32 ticks = system_tick;
    while (system_tick - ticks < 1) {
      if ((mem_get16(hda_base + 0x68) & 0x3) == 0b10) { break; }
    }
    if ((mem_get16(hda_base + 0x68) & 0x3) != 0b10) {
      mem_set16(hda_base + 0x68, 0b10);
      error("no response from hda");
      return 0;
    } else {
      mem_set16(hda_base + 0x68, 0b10);
    }
    return mem_get32(hda_base + 0x64);
  }
}
u8 hda_node_type(u32 codec, u32 node) {
  return (hda_verb(codec, node, 0xf00, 0x9) >> 20) & 0xf;
}
u8 hda_pin_complex_type(u32 codec, u32 node) {
  return (hda_verb(codec, node, 0xF1C, 0) >> 20) & 0xf;
}
u16 hda_get_connection_list_entry(u32 codec, u32 node, u32 index) {
  u32  cll        = hda_verb(codec, node, 0xf00, 0xe);
  bool long_entry = (cll & 0x80) != 0;
  info("cll: %08x and long_entry: %d", cll, long_entry);
  if (!long_entry) {
    return (hda_verb(codec, node, 0xf02, index / 4 * 4) >> ((index % 4) * 8)) & 0xff;
  } else {
    return (hda_verb(codec, node, 0xf02, index / 2 * 2) >> ((index % 2) * 16)) & 0xffff;
  }
  __builtin_unreachable();
  return 0;
}
void hda_pin_set_output_volume(u32 codec, u32 node, u32 cap, u32 volume) {
  u32 val  = (1 << 12) | (1 << 13);
  val     |= 0x8000;
  if (volume == 0 && cap & 0x80000000) {
    val |= (1 << 7);
  } else {
    val |= ((cap >> 8) & 0x7F) * volume / 100;
  }
  hda_verb(codec, node, 0x3, val);
}
void hda_init_audio_selector(u32 codec, u32 node);
void hda_init_audio_output(u32 codec, u32 node) {
  hda_verb(codec, node, 0x705, 0x0);
  hda_verb(codec, node, 0x708, 0x0);
  hda_verb(codec, node, 0x703, 0x0);
  hda_verb(codec, node, 0x706, 0x10);

  hda_pin_output_node = node;

  u32 amp_cap = hda_verb(codec, node, 0xf00, 0x12);
  hda_pin_set_output_volume(codec, node, amp_cap, 100);
  if (amp_cap != 0) {
    hda_pin_output_node    = node;
    hda_pin_output_amp_cap = amp_cap;
  }
  u32 pcm_format_cap = hda_verb(codec, node, 0xf00, 0xA);
  info("pcm format cap: %08x", pcm_format_cap);
  if (pcm_format_cap != 0) {
    hda_pin_pcm_format_cap = pcm_format_cap;
  } else {
    hda_pin_pcm_format_cap = hda_afg_pcm_format_cap;
    info("using afg pcm format cap: %08x", hda_afg_pcm_format_cap);
  }

  u32 stream_format_cap = hda_verb(codec, node, 0xf00, 0xB);
  if (stream_format_cap != 0) {
    hda_pin_stream_format_cap = stream_format_cap;
  } else {
    hda_pin_stream_format_cap = hda_afg_stream_format_cap;
  }
  if (hda_pin_output_amp_cap == 0) {
    hda_pin_output_node    = node;
    hda_pin_output_amp_cap = hda_afg_amp_cap;
  }
  info("successfully initialized audio output node %d S", hda_pin_output_node);
}
void hda_init_audio_mixer(u32 codec, u32 node) {
  hda_verb(codec, node, 0x705, 0x0);
  hda_verb(codec, node, 0x708, 0x0);

  u32 amp_cap = hda_verb(codec, node, 0xf00, 0x12);
  hda_pin_set_output_volume(codec, node, amp_cap, 100);
  if (amp_cap != 0) {
    hda_pin_output_node    = node;
    hda_pin_output_amp_cap = amp_cap;
  }
  u16 n = hda_get_connection_list_entry(codec, node, 0);
  info("n=%d", n);
  u8 type = hda_node_type(codec, n);
  if (type == HDA_WIDGET_AUDIO_OUTPUT) {
    info("(%08x) : node %d is Audio Output", type, n);
    hda_init_audio_output(codec, n);
  } else if (type == HDA_WIDGET_MIXER) {
    info("(%08x) : node %d is Mixer", type, n);
    hda_init_audio_mixer(codec, n);
  } else if (type == HDA_WIDGET_AUDIO_SELECTOR) {
    info("(%08x) : node %d is Audio Selector", type, n);
    hda_init_audio_selector(codec, n);
  }
}
void hda_init_audio_selector(u32 codec, u32 node) {
  hda_verb(codec, node, 0x705, 0x0);
  hda_verb(codec, node, 0x708, 0x0);
  hda_verb(codec, node, 0x703, 0x0);

  u32 amp_cap = hda_verb(codec, node, 0xf00, 0x12);
  hda_pin_set_output_volume(codec, node, amp_cap, 100);
  if (amp_cap != 0) {
    hda_pin_output_node    = node;
    hda_pin_output_amp_cap = amp_cap;
  }
  u16 n = hda_get_connection_list_entry(codec, node, 0);
  info("n=%d", n);
  u8 type = hda_node_type(codec, n);
  if (type == HDA_WIDGET_AUDIO_OUTPUT) {
    info("(%08x) : node %d is Audio Output", type, n);
    hda_init_audio_output(codec, n);
  }
}
void hda_init_output_pin(u32 codec, u32 node) {
  hda_verb(codec, node, 0x705, 0x0);
  hda_verb(codec, node, 0x708, 0x0);
  hda_verb(codec, node, 0x703, 0x0);

  hda_verb(codec, node, 0x707, hda_verb(codec, node, 0xf07, 0) | (1 << 6) | (1 << 7));
  hda_verb(codec, node, 0x70c, 1);

  u32 amp_cap = hda_verb(codec, node, 0xf00, 0x12);
  hda_pin_set_output_volume(codec, node, amp_cap, 100);
  if (amp_cap != 0) {
    hda_pin_output_node    = node;
    hda_pin_output_amp_cap = amp_cap;
  }
  hda_verb(codec, node, 0x701, 0);
  u16 n = hda_get_connection_list_entry(codec, node, 0);
  info("n=%d", n);
  u8 type = hda_node_type(codec, n);
  if (type == HDA_WIDGET_AUDIO_OUTPUT) {
    info("(%08x) : node %d is Audio Output", type, n);
    hda_init_audio_output(codec, n);
  } else if (type == HDA_WIDGET_MIXER) {
    info("(%08x) : node %d is Mixer", type, n);
    hda_init_audio_mixer(codec, n);
  } else if (type == HDA_WIDGET_AUDIO_SELECTOR) {
    info("(%08x) : node %d is Audio Selector", type, n);
    hda_init_audio_selector(codec, n);
  }
}

void hda_init_afg(u32 codec, u32 node) {
  hda_verb(codec, node, 0x7ff, 0);
  hda_verb(codec, node, 0x705, 0);
  hda_verb(codec, node, 0x708, 0);

  hda_afg_pcm_format_cap    = hda_verb(codec, node, 0xF00, 0xA);
  hda_afg_stream_format_cap = hda_verb(codec, node, 0xF00, 0xB);
  hda_afg_amp_cap           = hda_verb(codec, node, 0xF00, 0x12);

  info("pcm format cap: %08x", hda_afg_pcm_format_cap);
  info("stream format cap: %08x", hda_afg_stream_format_cap);
  info("amp cap: %08x", hda_afg_amp_cap);

  u32 count_raw  = hda_verb(codec, node, 0xF00, 0x4);
  u32 node_start = (count_raw >> 16) & 0xff;
  u32 node_count = count_raw & 0xff;
  info("(%08x) : node %d has %d subnodes from %d", count_raw, node, node_count, node_start);
  u32 pin_speaker   = 0;
  u32 pin_headphone = 0;
  u32 pin_output    = 0;
  for (int i = node_start; i < node_start + node_count; i++) {
    u8 type = hda_node_type(codec, i);
    if (type == HDA_WIDGET_AUDIO_OUTPUT) {
      info("(%08x) : node %d is Audio Output", type, i);
      hda_verb(codec, i, 0x706, 0x0);
    }
    if (type == HDA_WIDGET_PIN_COMPLEX) {
      info("(%08x) : node %d is Pin Complex", type, i);
      u8 pin_type = hda_pin_complex_type(codec, i);
      info("(%08x) : pin type is %d", pin_type, i);
      if (pin_type == HDA_PIN_COMPLEX_LINE_OUT) {
        info("(%08x) : node %d is Line Out", pin_type, i);
        pin_output = i;
      } else if (pin_type == HDA_PIN_COMPLEX_SPEAKER) {
        info("(%08x) : node %d is Speaker", pin_type, i);
        if (pin_speaker == 0) pin_speaker = i;
        if ((hda_verb(codec, i, 0xf00, 0x09) & 0b100) &&
            (hda_verb(codec, i, 0xf1c, 0) >> 30 & 0b11) != 1 &&
            (hda_verb(codec, i, 0xf00, 0x0c) & 0b10000)) {
          warn("the speaker is connected ready to output");
          pin_speaker = i;
        } else {
          warn("no output");
        }
      } else if (pin_type == HDA_PIN_COMPLEX_HP_OUT) {
        info("(%08x) : node %d is Headphone", pin_type, i);
        pin_headphone = i;
      } else if (pin_type == HDA_PIN_COMPLEX_CD) {

        info("(%08x) : node %d is CD", pin_type, i);
        pin_output = i;
      } else if (pin_type == HDA_PIN_COMPLEX_SPDIF_OUT) {
        info("(%08x) : node %d is SPDIF Out", pin_type, i);
        pin_output = i;
      } else if (pin_type == HDA_PIN_COMPLEX_DIG_OUT) {
        info("(%08x) : node %d is Digital Out", pin_type, i);
        pin_output = i;
      } else if (pin_type == HDA_PIN_COMPLEX_MODEM_LINE_SIDE) {
        info("(%08x) : node %d is Modem Line Side", pin_type, i);
        pin_output = i;
      } else if (pin_type == HDA_PIN_COMPLEX_MODEM_HANDSET_SIDE) {
        info("(%08x) : node %d is Modem Handset Side", pin_type, i);

        pin_output = i;
      }
    }
  }
  info("pin_speaker: %d, pin_headphone: %d, pin_output: %d", pin_speaker, pin_headphone,
       pin_output);
  if (pin_speaker) {
    hda_init_output_pin(codec, pin_speaker);
    if (!pin_headphone) return;
    // TODO: 处理耳机
  } else if (pin_headphone) {
    hda_init_output_pin(codec, pin_headphone);
  } else if (pin_output) {
    hda_init_output_pin(codec, pin_output);
  }
}

void hda_init_codec(u32 codec) {
  u32 count_raw  = hda_verb(codec, 0, 0xf00, 0x4);
  u32 node_start = (count_raw >> 16) & 0xff;
  u32 node_count = count_raw & 0xff;
  info("(%08x) : codec %d has %d nodes from %d", count_raw, codec, node_count, node_start);
  for (int i = node_start; i < node_start + node_count; i++) {
    u32 type = hda_verb(codec, i, 0xf00, 0x5);
    if ((type & 0xff) == 0x1) {
      info("(%08x) : node %d is Audio Function Group", type, i);
      hda_init_afg(codec, i);
      return;
    }
  }
}

void pci_set_drive_irq(u8 bus, u8 slot, u8 func, u8 irq);
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
  write_pci(hda_bus, hda_slot, hda_func, 0x04,
            ((read_pci(hda_bus, hda_slot, hda_func, 0x04) & ~((u32)1 << 10)) | (1 << 2) |
             (1 << 1))); // enable interrupts, enable bus mastering, enable MMIO space
  hda_buffer_ptr = page_alloc(4096 * 2);
  info("hda card found at bus %d slot %d func %d", hda_bus, hda_slot, hda_func);
  hda_base = read_bar_n(hda_bus, hda_slot, hda_func, 0);
  info("hda base address: 0x%x", hda_base);
  mem_set32(hda_base + 0x08, 0);
  int ticks = system_tick;
  while (system_tick - ticks < 1) {
    if ((mem_get32(hda_base + 0x08) & 0x01) == 0) { break; }
  }
  if ((mem_get32(hda_base + 0x08) & 0x01) != 0) {
    error("hda reset failed");
    return;
  }
  mem_set32(hda_base + 0x08, 1);
  ticks = system_tick;
  while (system_tick - ticks < 1) {
    if ((mem_get32(hda_base + 0x08) & 0x01) == 1) { break; }
  }
  if ((mem_get32(hda_base + 0x08) & 0x01) != 1) {
    error("hda reset failed");
    return;
  }
  info("hda card is working now");
  int input_stream_count = (mem_get16(hda_base + 0x00) >> 8) & 0x0f;
  info("input stream count: %d", input_stream_count);
  output_base = hda_base + 0x80 + (0x20 * input_stream_count);
  info("output base address: 0x%x", output_base);
  output_buffer = page_malloc_one();

  int irq = pci_get_drive_irq(hda_bus, hda_slot, hda_func);
  irq_enable(irq);
  prev_handler = inthandler_set(0x20 + irq, hda_interrupt_handler);
  mem_set32(hda_base + 0x20, ((u32)1 << 31) | ((u32)1 << input_stream_count));

  info("%x", pci_get_drive_irq(hda_bus, hda_slot, hda_func));
  mem_set32(hda_base + 0x70, 0);
  mem_set32(hda_base + 0x74, 0);

  mem_set32(hda_base + 0x34, 0);
  mem_set32(hda_base + 0x38, 0);

  mem_set8(hda_base + 0x4c, 0);
  mem_set8(hda_base + 0x5c, 0);

  corb = page_malloc_one();
  mem_set32(hda_base + 0x40, (u32)corb);
  mem_set32(hda_base + 0x44, 0);

  u8 corb_size = mem_get8(hda_base + 0x4e) >> 4 & 0x0f;
  if (corb_size & 0b0001) {
    corb_entry_count = 2;
    mem_set8(hda_base + 0x4e, 0b00);
  } else if (corb_size & 0b0010) {
    corb_entry_count = 16;
    mem_set8(hda_base + 0x4e, 0b01);
  } else if (corb_size & 0b0100) {
    corb_entry_count = 256;
    mem_set8(hda_base + 0x4e, 0b10);
  } else {
    error("corb size not supported");
    goto mmio;
  }
  info("corb size: %d entries", corb_entry_count);

  mem_set16(hda_base + 0x4A, 0x8000);
  ticks = system_tick;
  while (system_tick - ticks < 1) {

    if ((mem_get16(hda_base + 0x4A) & 0x8000)) { break; }
  }
  if ((mem_get16(hda_base + 0x4A) & 0x8000) == 0) {
    error("corb reset failed");
    goto mmio;
  }
  mem_set16(hda_base + 0x4A, 0x0000);
  ticks = system_tick;
  while (system_tick - ticks < 1) {
    if ((mem_get16(hda_base + 0x4A) & 0x8000) == 0) { break; }
  }
  if ((mem_get16(hda_base + 0x4A) & 0x8000) != 0) {
    error("corb reset failed");
    goto mmio;
  }
  mem_set16(hda_base + 0x48, 0);

  corb_write_pointer = 1;
  info("corb has been reset already");

  rirb = page_malloc_one();
  mem_set32(hda_base + 0x50, (u32)rirb);
  mem_set32(hda_base + 0x54, 0);

  u8 rirb_size = mem_get8(hda_base + 0x5e) >> 4 & 0x0f;
  if (rirb_size & 0b0001) {
    rirb_entry_count = 2;
    mem_set8(hda_base + 0x5e, 0b00);
  } else if (rirb_size & 0b0010) {
    rirb_entry_count = 16;
    mem_set8(hda_base + 0x5e, 0b01);
  } else if (rirb_size & 0b0100) {
    rirb_entry_count = 256;
    mem_set8(hda_base + 0x5e, 0b10);
  } else {
    error("rirb size not supported");
    goto mmio;
  }
  info("rirb size: %d entries", rirb_entry_count);

  mem_set16(hda_base + 0x58, 0x8000);
  wait(1);

  mem_set16(hda_base + 0x5A, 0x0000);

  rirb_read_pointer = 1;
  info("rirb has been reset already");

  mem_set8(hda_base + 0x4c, 0b10);
  mem_set8(hda_base + 0x5c, 0b10);
  info("corb rirb dma has been started");
  send_verb_method = SEND_VERB_METHOD_DMA;
  for (int i = 0; i < 16; i++) {
    u32 codec_id = hda_verb(i, 0, 0xf00, 0);
    if (codec_id != 0) {
      hda_codec_number = i;
      hda_init_codec(i);
      return;
    }
  }
mmio:
  warn("use mmio");
  send_verb_method = SEND_VERB_METHOD_MMIO;
  mem_set8(output_base + 0x4c, 0);
  mem_set8(output_base + 0x5c, 0);
  for (int i = 0; i < 16; i++) {
    u32 codec_id = hda_verb(i, 0, 0xf00, 0);
    if (codec_id != 0) {
      hda_codec_number = i;
      hda_init_codec(i);
      return;
    }
  }
}

u16 hda_return_sound_data_format(u32 sample_rate, u32 channels, u32 bits_per_sample) {
  u16 data_format = 0;

  // channels
  data_format = (channels - 1);

  // bits per sample
  switch (bits_per_sample) {
  case 16: data_format |= (0b001 << 4); break;
  case 20: data_format |= (0b010 << 4); break;
  case 24: data_format |= (0b011 << 4); break;
  case 32: data_format |= (0b100 << 4); break;
  default: /* Handle invalid bits_per_sample if necessary */ break;
  }

  // sample rate
  switch (sample_rate) {
  case 48000: data_format |= (0b0000000 << 8); break;
  case 44100: data_format |= (0b1000000 << 8); break;
  case 32000: data_format |= (0b0001010 << 8); break;
  case 22050: data_format |= (0b1000001 << 8); break;
  case 16000: data_format |= (0b0000010 << 8); break;
  case 11025: data_format |= (0b1000011 << 8); break;
  case 8000: data_format |= (0b0000101 << 8); break;
  case 88200: data_format |= (0b1001000 << 8); break;
  case 96000: data_format |= (0b0001000 << 8); break;
  case 176400: data_format |= (0b1011000 << 8); break;
  case 192000: data_format |= (0b0011000 << 8); break;
  default: /* Handle invalid sample_rate if necessary */ break;
  }

  return data_format;
}

void hda_play_pcm(void *buffer, u32 size, u32 sample_rate, u32 channels, u32 bits_per_sample) {
  u16 data_format = hda_return_sound_data_format(sample_rate, channels, bits_per_sample);
  if (!(hda_pin_stream_format_cap & 1)) {
    error("pcm format not supported");
    return;
  }
  int ticks = system_tick;
  mem_set8(output_base + 0x0, 0);
  while (system_tick - ticks < 1) {
    if ((mem_get8(output_base + 0x0) & 0b11) == 0x0) { break; }
  }
  if ((mem_get8(output_base + 0x0) & 0b11) != 0x0) {
    error("output reset failed 1");
    return;
  }
  mem_set8(output_base + 0x0, 1);
  ticks = system_tick;
  while (system_tick - ticks < 1) {
    if ((mem_get8(output_base + 0x0) & 0b01) == 0x1) { break; }
  }
  if ((mem_get8(output_base + 0x0) & 0b01) != 0x1) {
    error("stream reset failed 2");
    return;
  }
  wait(1);

  ticks = system_tick;
  mem_set8(output_base + 0x0, 0);
  while (system_tick - ticks < 1) {
    if ((mem_get8(output_base + 0x0) & 0b11) == 0x0) { break; }
  }
  if ((mem_get8(output_base + 0x0) & 0b11) != 0x0) {
    error("output reset failed");
    return;
  }
  wait(1);

  mem_set8(output_base + 0x3, 0b11100);
  explicit_bzero(output_buffer, 16 * 2);
  output_buffer[0] = (u32)buffer;
  output_buffer[2] = size;
  output_buffer[3] = 1;

  output_buffer[4] = (u32)buffer + size;
  output_buffer[6] = size;
  output_buffer[7] = 1;

  asm_wbinvd;

  mem_set32(output_base + 0x18, (u32)output_buffer);
  mem_set32(output_base + 0x1c, 0);

  mem_set32(output_base + 0x8, size * 2);
  mem_set16(output_base + 0xc, 1);
  mem_set16(output_base + 0x12, data_format);
  // printf("data_format = %x %d\n", data_format, hda_pin_output_node);
  hda_verb(hda_codec_number, hda_pin_output_node, 0x2, data_format);
  wait(1);

  mem_set8(output_base + 0x2, 0x1c);

  mem_set8(output_base + 0x0, 0b110);
}

void hda_stop() {
  mem_set8(output_base + 0x0, 0);
}
void hda_continue() {
  mem_set8(output_base + 0x0, 0b110);
}
u32 hda_get_bytes_sent() {
  return mem_get32(output_base + 0x4);
}

u8 hda_is_supported_sample_rate(u32 sample_rate) {
  u32 sample_rates[11] = {8000,  11025, 16000, 22050,  32000, 44100,
                          48000, 88200, 96000, 176400, 192000};
  u16 mask             = 0x0000001;

  //get bit of requested sample rate in capabilities
  for (int i = 0; i < 11; i++) {
    if (sample_rates[i] == sample_rate) { break; }
    mask <<= 1;
  }

  if ((hda_pin_pcm_format_cap & mask) == mask) {
    return 1;
  } else {
    return 0;
  }
}

#define HDA_BUF_SIZE 4096

static bool     hda_stopping = false;
static vsound_t snd;
static task_t   use_task;

static int hda_open(vsound_t vsound) {
  klogd("hda open has been called");
  use_task = current_task;
  return 0;
}

__attr(fastcall) void hda_interrupt_handler(i32 id, regs32 *regs) {
  // printf("hda interrupt has been called");
  if (prev_handler) { prev_handler(id, regs); }
  bool result = pci_check_interrupt_status(hda_bus, hda_slot, hda_func);
  // 不是我们的中断我们不要
  if (!result) { return; }
  if (hda_stopping) {
    hda_stop();
  } else {
    vsound_played(snd);
  }
  asm_wbinvd;

  mem_set8(output_base + 0x3, 1 << 2);
  task_run(use_task);
}

static void hda_close(vsound_t snd) {
  if (!snd->is_dma_ready) {
    hda_stopping = true;
  } else {
    hda_stop();
  }
}

static int hda_start_dma(vsound_t snd, void *addr) {
  if (snd->is_dma_ready) return 0;
  hda_play_pcm(addr, HDA_BUF_SIZE, snd->rate, snd->channels, sound_fmt_bytes(snd->fmt) * 8);
  return 0;
}

static struct vsound vsound = {
    .is_output = true,
    .name      = "hda",
    .open      = hda_open,
    .close     = hda_close,
    .start_dma = hda_start_dma,
    .bufsize   = HDA_BUF_SIZE,
};

static const i16 fmts[] = {
    SOUND_FMT_S16, SOUND_FMT_U16, SOUND_FMT_U32, SOUND_FMT_S32, -1,
};

static const i32 rates[] = {
    8000, 11025, 16000, 22050, 24000, 32000, 44100, 47250, 48000, 50000, -1,
};

void hda_regist() {
  snd = &vsound;
  if (!vsound_regist(snd)) {
    klogw("注册 hda 失败");
    return;
  }
  vsound_set_supported_fmts(snd, fmts, -1);
  vsound_set_supported_rates(snd, rates, -1);
  vsound_set_supported_ch(snd, 1);
  vsound_set_supported_ch(snd, 2);
  vsound_addbuf(snd, hda_buffer_ptr);
  vsound_addbuf(snd, hda_buffer_ptr + HDA_BUF_SIZE);
}
