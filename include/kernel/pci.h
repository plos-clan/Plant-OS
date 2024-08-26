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
void init_pci(void * adder_Base);
void pci_classcode_print(struct pci_config_space_public *pci_config_space_puclic);