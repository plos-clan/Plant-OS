#include <kernel.h>

bool vmware_backdoor_available = false;

enum hypervisor_id hypervisor_id = HYPERVISOR_UNKNOWN;

static void detect_hypervisor_id() {
  static const struct {
    const void        *key;
    enum hypervisor_id value;
  } hypervisor_ids[] = {
      {"KVMKVMKVM\0\0\0", HYPERVISOR_KVM   },
      {"Linux KVM Hv",    HYPERVISOR_KVM_HV},
      {"TCGTCGTCGTCG",    HYPERVISOR_QEMU  },
      {"VMwareVMware",    HYPERVISOR_VMWARE},
      {"VBoxVBoxVBox",    HYPERVISOR_VBOX  },
      {"Microsoft Hv",    HYPERVISOR_HYPERV},
  };
  for (usize i = 0; i < lengthof(hypervisor_ids); i++) {
    if (memeq(cpuids.hypervisor_id, hypervisor_ids[i].key, 12)) {
      hypervisor_id = hypervisor_ids[i].value;
      return;
    }
  }
}

void detect_hypervisor() {
  detect_hypervisor_id();
  vmware_backdoor_available = vm_is_vmware;
}

static inthandler_f inthandler2c;

static void *ps2_mouse_handler = null;

static void vmware_mouse_init() {
  // 取消鼠标捕获
  vmware_send(41, 0x45414552);
  vmware_send(40, 0);
  vmware_send(39, 1);
  vmware_send(41, 0x53424152);
  ps2_mouse_handler = inthandler_set(0x2c, inthandler2c);
}

static void vmware_mouse_deinit() {
  vmware_send(41, 0xF5);
  inthandler_set(0x2c, ps2_mouse_handler);
}

void vmtools_init() {
  if (vmware_backdoor_available) {
    vmware_mouse_init(); //
  }
}

static PointerEvent event;

static FASTCALL void inthandler2c(i32 id, regs32 *regs) {
  asm_in8(PORT_KEYDAT);

  val status = vmware_send(40, 0);

  if (status.eax == 0xFFFF0000) {
    klogw("VMware mouse data error");
    vmware_mouse_deinit();
    vmware_mouse_init();
    return;
  }

  if ((status.eax & 0xFFFF) < 4) return;

  val data = vmware_send(39, 4);

  val flags   = (data.eax & 0xFFFF0000) >> 16; /* Not important */
  val buttons = data.eax & 0xFFFF;             /* 0x10 = Right, 0x20 = Left, 0x08 = Middle */
  val x       = data.ebx;                      /* Both X and Y are scaled from 0 to 0xFFFF */
  val y       = data.ecx;     /* You should map these somewhere to the actual resolution. */
  val z       = (i8)data.edx; /* Z is a single signed byte indicating scroll direction. */
  klogd("mouse x: %d, y: %d, z: %d", x, y, z);

  event.buttons = 0;
  if (buttons & 0x20) event.buttons |= MASK(0);
  if (buttons & 0x08) event.buttons |= MASK(1);
  if (buttons & 0x10) event.buttons |= MASK(2);
  event.xratio = (f32)x / 0xFFFF;
  event.yratio = (f32)y / 0xFFFF;
}
