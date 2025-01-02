#include <kernel.h>

inthandler_t old_irqhandler;

static byte bus, slot, func;
static u32  mmio_base;

static inthandler_f irqhandler;

void virtio_gpu_init() {
  val ret = pci_get_device(0x1af4, 0x1050, 0x1100, &bus, &slot, &func);
  if (ret < 0) {
    klogi("virtio-gpu: device not found");
    return;
  }
  val irq        = pci_get_drive_irq(bus, slot, func);
  old_irqhandler = inthandler_set(0x20 + irq, irqhandler);

  for (uint8_t i = 0; i < 6; i++) {
    val off       = 0x10 + i * 4;
    val bar_value = read_pci(bus, slot, func, off);
    if ((bar_value & 1) == 0) {
      mmio_base = bar_value & ~0xf;
      klogd("VirtIO GPU MMIO Base Address: 0x%08X\n", mmio_base);
      break;
    }
  }
}

__attr(fastcall) void irqhandler(i32 id, regs32 *regs) {
  if (old_irqhandler) old_irqhandler(id, regs);
}
