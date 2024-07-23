#include <dosldr.h>
#define PCI_COMMAND_PORT 0xCF8
#define PCI_DATA_PORT    0xCFC
#define mem_mapping      0
#define input_output     1
typedef struct base_address_register {
  int prefetchable;
  u8 *address;
  u32 size;
  int type;
} base_address_register;
u32 read_pci(u8 bus, u8 device, u8 function, u8 registeroffset) {
  u32 id = 1 << 31 | ((bus & 0xff) << 16) | ((device & 0x1f) << 11) | ((function & 0x07) << 8) |
           (registeroffset & 0xfc);
  asm_out32(PCI_COMMAND_PORT, id);
  u32 result = asm_in32(PCI_DATA_PORT);
  return result >> (8 * (registeroffset % 4));
}
u32 read_bar_n(u8 bus, u8 device, u8 function, u8 bar_n) {
  u32 bar_offset = 0x10 + 4 * bar_n;
  return read_pci(bus, device, function, bar_offset);
}
void write_pci(u8 bus, u8 device, u8 function, u8 registeroffset, u32 value) {
  u32 id = 1 << 31 | ((bus & 0xff) << 16) | ((device & 0x1f) << 11) | ((function & 0x07) << 8) |
           (registeroffset & 0xfc);
  asm_out32(PCI_COMMAND_PORT, id);
  asm_out32(PCI_DATA_PORT, value);
}
u32 pci_read_command_status(u8 bus, u8 slot, u8 func) {
  return read_pci(bus, slot, func, 0x04);
}
// write command status register
void pci_write_command_status(u8 bus, u8 slot, u8 func, u32 value) {
  write_pci(bus, slot, func, 0x04, value);
}
base_address_register get_base_address_register(u8 bus, u8 device, u8 function, u8 bar) {
  base_address_register result;

  u32 headertype = read_pci(bus, device, function, 0x0e) & 0x7e;
  int max_bars   = 6 - 4 * headertype;
  if (bar >= max_bars) return result;

  u32 bar_value = read_pci(bus, device, function, 0x10 + 4 * bar);
  result.type   = (bar_value & 1) ? input_output : mem_mapping;

  if (result.type == mem_mapping) {
    switch ((bar_value >> 1) & 0x3) {
    case 0: // 32
    case 1: // 20
    case 2: // 64
      break;
    }
    result.address      = (u8 *)(bar_value & ~0x3);
    result.prefetchable = 0;
  } else {
    result.address      = (u8 *)(bar_value & ~0x3);
    result.prefetchable = 0;
  }
  return result;
}
u8 pci_get_drive_irq(u8 bus, u8 slot, u8 func) {
  return (u8)read_pci(bus, slot, func, 0x3c);
}
u32 pci_get_port_base(u8 bus, u8 slot, u8 func) {
  u32 io_port = 0;
  for (int i = 0; i < 6; i++) {
    base_address_register bar = get_base_address_register(bus, slot, func, i);
    if (bar.type == input_output) { io_port = (u32)bar.address; }
  }
  return io_port;
}
void pci_config(u32 bus, u32 f, u32 equipment, u32 adder) {
  u32 cmd = 0;
  cmd     = 0x80000000 + (u32)adder + ((u32)f << 8) + ((u32)equipment << 11) + ((u32)bus << 16);
  // cmd = cmd | 0x01;
  asm_out32(PCI_COMMAND_PORT, cmd);
}
void init_PCI(u32 adder_Base) {
  u32 i, BUS, Equipment, F, ADDER, *i1;
  u8 *PCI_DATA = (u8 *)adder_Base, *PCI_DATA1;
  for (BUS = 0; BUS < 256; BUS++) {                    //查询总线
    for (Equipment = 0; Equipment < 32; Equipment++) { //查询设备
      for (F = 0; F < 8; F++) {                        //查询功能
        pci_config(BUS, F, Equipment, 0);
        if (asm_in32(PCI_DATA_PORT) != 0xFFFFFFFF) {
          //当前插槽有设备
          //把当前设备信息映射到PCI数据区
          int key = 1;
          while (key) {
            //此配置表为空
            // printf("PCI_DATA:%x\n", PCI_DATA);
            // getch();
            PCI_DATA1  = PCI_DATA;
            *PCI_DATA1 = 0xFF; //表占用标志
            PCI_DATA1++;
            *PCI_DATA1 = BUS; //总线号
            PCI_DATA1++;
            *PCI_DATA1 = Equipment; //设备号
            PCI_DATA1++;
            *PCI_DATA1 = F; //功能号
            PCI_DATA1++;
            PCI_DATA1 = PCI_DATA1 + 8;
            //写入寄存器配置
            for (ADDER = 0; ADDER < 256; ADDER = ADDER + 4) {
              pci_config(BUS, F, Equipment, ADDER);
              i  = asm_in32(PCI_DATA_PORT);
              i1 = (u32 *)i;
              //*i1 = PCI_DATA1;
              memcpy(PCI_DATA1, &i, 4);
              PCI_DATA1 = PCI_DATA1 + 4;
            }
            for (u8 barNum = 0; barNum < 6; barNum++) {
              base_address_register bar = get_base_address_register(BUS, Equipment, F, barNum);
              if (bar.address && (bar.type == input_output)) {
                PCI_DATA1 += 4;
                int i      = ((u32)(bar.address));
                memcpy(PCI_DATA1, &i, 4);
              }
            }
            /*PCI_DATA += 12;
            struct PCI_CONFIG_SPACE_PUCLIC *PCI_CONFIG_SPACE = (struct
            PCI_CONFIG_SPACE_PUCLIC *)PCI_DATA; PCI_DATA -= 12;
            printf("PCI_CONFIG_SPACE:%08x\n", PCI_CONFIG_SPACE);
            printf("PCI_CONFIG_SPACE->VendorID:%08x\n",
            PCI_CONFIG_SPACE->VendorID);
            printf("PCI_CONFIG_SPACE->DeviceID:%08x\n",
            PCI_CONFIG_SPACE->DeviceID);
            printf("PCI_CONFIG_SPACE->Command:%08x\n",
            PCI_CONFIG_SPACE->Command);
            printf("PCI_CONFIG_SPACE->Status:%08x\n", PCI_CONFIG_SPACE->Status);
            printf("PCI_CONFIG_SPACE->RevisionID:%08x\n",
            PCI_CONFIG_SPACE->RevisionID);
            printf("PCI_CONFIG_SPACE->ProgIF:%08x\n", PCI_CONFIG_SPACE->ProgIF);
            printf("PCI_CONFIG_SPACE->SubClass:%08x\n",
            PCI_CONFIG_SPACE->SubClass);
            printf("PCI_CONFIG_SPACE->BaseCode:%08x\n",
            PCI_CONFIG_SPACE->BaseClass);
            printf("PCI_CONFIG_SPACE->CacheLineSize:%08x\n",
            PCI_CONFIG_SPACE->CacheLineSize);
            printf("PCI_CONFIG_SPACE->LatencyTimer:%08x\n",
            PCI_CONFIG_SPACE->LatencyTimer);
            printf("PCI_CONFIG_SPACE->HeaderType:%08x\n",
            PCI_CONFIG_SPACE->HeaderType);
            printf("PCI_CONFIG_SPACE->BIST:%08x\n", PCI_CONFIG_SPACE->BIST);
            printf("PCI_CONFIG_SPACE->BaseAddr0:%08x\n",
            PCI_CONFIG_SPACE->BaseAddr[0]);
            printf("PCI_CONFIG_SPACE->BaseAddr1:%08x\n",
            PCI_CONFIG_SPACE->BaseAddr[1]);
            printf("PCI_CONFIG_SPACE->BaseAddr2:%08x\n",
            PCI_CONFIG_SPACE->BaseAddr[2]);
            printf("PCI_CONFIG_SPACE->BaseAddr3:%08x\n",
            PCI_CONFIG_SPACE->BaseAddr[3]);
            printf("PCI_CONFIG_SPACE->BaseAddr4:%08x\n",
            PCI_CONFIG_SPACE->BaseAddr[4]);
            printf("PCI_CONFIG_SPACE->BaseAddr5:%08x\n",
            PCI_CONFIG_SPACE->BaseAddr[5]);
            printf("PCI_CONFIG_SPACE->CardbusCISPtr:%08x\n",
            PCI_CONFIG_SPACE->CardbusCIS);
            printf("PCI_CONFIG_SPACE->SubsystemVendorID:%08x\n",
            PCI_CONFIG_SPACE->SubVendorID);
            printf("PCI_CONFIG_SPACE->SubsystemID:%08x\n",
            PCI_CONFIG_SPACE->SubSystemID);
            printf("PCI_CONFIG_SPACE->ExpansionROMBaseAddr:%08x\n",
            PCI_CONFIG_SPACE->ROMBaseAddr);
            printf("PCI_CONFIG_SPACE->CapabilitiesPtr:%08x\n",
            PCI_CONFIG_SPACE->CapabilitiesPtr);
            printf("PCI_CONFIG_SPACE->Reserved1:%08x\n",
            PCI_CONFIG_SPACE->Reserved[0]);
            printf("PCI_CONFIG_SPACE->Reserved2:%08x\n",
            PCI_CONFIG_SPACE->Reserved[1]);
            printf("PCI_CONFIG_SPACE->InterruptLine:%08x\n",
            PCI_CONFIG_SPACE->InterruptLine);
            printf("PCI_CONFIG_SPACE->InterruptPin:%08x\n",
            PCI_CONFIG_SPACE->InterruptPin);
            printf("PCI_CONFIG_SPACE->MinGrant:%08x\n",
            PCI_CONFIG_SPACE->MinGrant);
            printf("PCI_CONFIG_SPACE->MaxLatency:%08x\n",
            PCI_CONFIG_SPACE->MaxLatency); for (int i = 0; i < 272+4; i++)
            {
                printf("%02x ", PCI_DATA[i]);
            }
            printf("\n");*/
            PCI_DATA = PCI_DATA + 0x110 + 4;
            key      = 0;
          }
        }
      }
    }
  }
  //函数执行完PCI_DATA就是PCI设备表的结束地址
}
void PCI_ClassCode_Print(struct pci_config_space_public *pci_config_space_puclic) {
  u8 *pci_drive = (u8 *)pci_config_space_puclic - 12;
  logf("BUS:%02x ", pci_drive[1]);
  logf("EQU:%02x ", pci_drive[2]);
  logf("F:%02x ", pci_drive[3]);
  logf("IO Port:%08x ", pci_get_port_base(pci_drive[1], pci_drive[2], pci_drive[3]));
  logf("IRQ Line:%02x ", pci_get_drive_irq(pci_drive[1], pci_drive[2], pci_drive[3]));
  if (pci_config_space_puclic->BaseClass == 0x0) {
    logf("Nodefined ");
    if (pci_config_space_puclic->SubClass == 0x0)
      logf("Non-VGA-Compatible Unclassified Device\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      logf("VGA-Compatible Unclassified Device\n");
  } else if (pci_config_space_puclic->BaseClass == 0x1) {
    logf("Mass Storage Controller ");
    if (pci_config_space_puclic->SubClass == 0x0)
      logf("SCSI Bus Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      logf("IDE Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      logf("Floppy Disk Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x3)
      logf("IPI Bus Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x4)
      logf("RAID Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x5)
      logf("ATA Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x6)
      logf("Serial ATA Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x7)
      logf("Serial Attached SCSI Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x8)
      logf("Non-Volatile Memory Controller\n");
    else
      logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x2) {
    logf("Network Controller ");
    if (pci_config_space_puclic->SubClass == 0x0)
      logf("Ethernet Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      logf("Token Ring Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      logf("FDDI Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x3)
      logf("ATM Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x4)
      logf("ISDN Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x5)
      logf("WorldFip Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x6)
      logf("PICMG 2.14 Multi Computing Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x7)
      logf("Infiniband Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x8)
      logf("Fabric Controller\n");
    else
      logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x3) {
    logf("Display Controller ");
    if (pci_config_space_puclic->SubClass == 0x0)
      logf("VGA Compatible Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      logf("XGA Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      logf("3D Controller (Not VGA-Compatible)\n");
    else
      logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x4) {
    logf("Multimedia Controller ");
    logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x5) {
    logf("Memory Controller ");
    logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x6) {
    logf("Bridge ");
    if (pci_config_space_puclic->SubClass == 0x0)
      logf("Host Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      logf("ISA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      logf("EISA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x3)
      logf("MCA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x4 || pci_config_space_puclic->SubClass == 0x9)
      logf("PCI-to-PCI Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x5)
      logf("PCMCIA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x6)
      logf("NuBus Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x7)
      logf("CardBus Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x8)
      logf("RACEway Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0xA)
      logf("InfiniBand-to-PCI Host Bridge\n");
    else
      logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x7) {
    logf("Simple Communication Controller ");
    logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x8) {
    logf("Base System Peripheral ");
    logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x9) {
    logf("Input Device Controller ");
    logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0xA) {
    logf("Docking Station ");
    logf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0xB) {
    logf("Processor ");
    logf("\n");
  } else {
    logf("Unknow\n");
  }
}