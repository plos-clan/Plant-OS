#pragma once
#include <define.h>

#define vmware_send(cmd, arg)                                                                      \
  ({                                                                                               \
    const u32 _cmd_ = (cmd);                                                                       \
    const u32 _arg_ = (arg);                                                                       \
    struct {                                                                                       \
      u32 eax, ebx, ecx, edx, esi, edi;                                                            \
    } _564D5868_ = {0x564D5868, _arg_, _cmd_, 0x5658, 0, 0};                                       \
    asm volatile("in %%dx, %%eax\n\t"                                                              \
                 : "+a"(_564D5868_.eax), "+b"(_564D5868_.ebx), "+c"(_564D5868_.ecx),               \
                   "+d"(_564D5868_.edx), "+S"(_564D5868_.esi), "+D"(_564D5868_.edi));              \
    _564D5868_;                                                                                    \
  })

#define vm_is_vmware (vmware_send(0x0A, 0).ebx == 0x564D5868)

enum hypervisor_id {
  HYPERVISOR_UNKNOWN,
  HYPERVISOR_KVM,
  HYPERVISOR_KVM_HV,
  HYPERVISOR_QEMU,
  HYPERVISOR_VMWARE,
  HYPERVISOR_VBOX,
  HYPERVISOR_HYPERV,
};

extern enum hypervisor_id hypervisor_id;

// 注意非 vmwware 的虚拟机也有可能提供 vmware backdoor
extern bool vmware_backdoor_available;

void detect_hypervisor();

void vmtools_init();
