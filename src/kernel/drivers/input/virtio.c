#include <kernel.h>

void virtio_init() {
  byte bus, slot, func;
  pci_get_device(0x1af4, 0x1052, &bus, &slot, &func);
  pci_get_drive_irq(bus, slot, func);
}
