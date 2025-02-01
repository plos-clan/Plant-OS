#pragma once
#include <define.h>
struct pci_config_space_public {
  u16 VendorID;
  u16 DeviceID;
  u16 Command;
  u16 Status;
  u8  RevisionID;
  u8  ProgIF;
  u8  SubClass;
  u8  BaseClass;
  u8  CacheLineSize;
  u8  LatencyTimer;
  u8  HeaderType;
  u8  BIST;
  u32 BaseAddr[6];
  u32 CardbusCIS;
  u16 SubVendorID;
  u16 SubSystemID;
  u32 ROMBaseAddr;
  u8  CapabilitiesPtr;
  u8  Reserved[3];
  u32 Reserved1;
  u8  InterruptLine;
  u8  InterruptPin;
  u8  MinGrant;
  u8  MaxLatency;
};

void pci_init();
void pci_classcode_print(const struct pci_config_space_public *pci_config_space_puclic);
int  pci_get_device(int vendor_id, int device_id, int subsystem_id, u8 *bus, u8 *slot, u8 *func);
u32  read_bar_n(u8 bus, u8 device, u8 function, u8 bar_n);
void write_pci(u8 bus, u8 device, u8 function, u8 registeroffset, u32 value);
u32  read_pci(u8 bus, u8 device, u8 function, u8 registeroffset);
u8   pci_get_drive_irq(u8 bus, u8 slot, u8 func);
bool pci_check_interrupt_status(u8 bus, u8 slot, u8 func);
void pci_write_command_status(u8 bus, u8 slot, u8 func, u32 value);
u32  pci_read_command_status(u8 bus, u8 slot, u8 func);