#include <kernel.h>

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
  u32 id = (u32)1 << 31                    //
           | (((u32)bus & 0xff) << 16)     //
           | (((u32)device & 0x1f) << 11)  //
           | (((u32)function & 0x07) << 8) //
           | ((u32)registeroffset & 0xfc);
  asm_out32(PCI_COMMAND_PORT, id);
  u32 result = asm_in32(PCI_DATA_PORT);
  return result >> (8 * (registeroffset % 4));
}

u32 read_bar_n(u8 bus, u8 device, u8 function, u8 bar_n) {
  u32 bar_offset = 0x10 + 4 * bar_n;
  return read_pci(bus, device, function, bar_offset);
}

void write_pci(u8 bus, u8 device, u8 function, u8 registeroffset, u32 value) {
  u32 id = (u32)1 << 31                    //
           | (((u32)bus & 0xff) << 16)     //
           | (((u32)device & 0x1f) << 11)  //
           | (((u32)function & 0x07) << 8) //
           | ((u32)registeroffset & 0xfc);
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

static base_address_register get_base_address_register(u8 bus, u8 device, u8 function, u8 bar) {
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
    result.address      = (u8 *)(bar_value & ~(u32)0x3);
    result.prefetchable = 0;
  } else {
    result.address      = (u8 *)(bar_value & ~(u32)0x3);
    result.prefetchable = 0;
  }
  return result;
}

u8 pci_get_drive_irq(u8 bus, u8 slot, u8 func) {
  return (u8)read_pci(bus, slot, func, 0x3c);
}

void pci_set_drive_irq(u8 bus, u8 slot, u8 func, u8 irq) {
  write_pci(bus, slot, func, 0x3c, read_pci(bus, slot, func, 0x3c) & ~(0xff) | irq);
}

u32 pci_get_port_base(u8 bus, u8 slot, u8 func) {
  u32 io_port = 0;
  for (int i = 0; i < 6; i++) {
    base_address_register bar = get_base_address_register(bus, slot, func, i);
    if (bar.type == input_output) { io_port = (u32)bar.address; }
  }
  return io_port;
}

void pci_get_device(u16 vendor_id, u16 device_id, u8 *bus, u8 *slot, u8 *func) {
  extern void *pci_addr_base;
  u8          *pci_drive = pci_addr_base;
  for (;; pci_drive += 0x110 + 4) {
    if (pci_drive[0] == 0xff) {
      var pci_config_space_puclic = (struct pci_config_space_public *)(pci_drive + 0x0c);
      if (pci_config_space_puclic->VendorID == vendor_id &&
          pci_config_space_puclic->DeviceID == device_id) {
        *bus  = pci_drive[1];
        *slot = pci_drive[2];
        *func = pci_drive[3];
        return;
      }
    } else {
      break;
    }
  }
}

void pci_config(u32 bus, u32 f, u32 equipment, u32 adder) {
  u32 cmd = 0;
  cmd     = 0x80000000 + (u32)adder + ((u32)f << 8) + ((u32)equipment << 11) + ((u32)bus << 16);
  // cmd = cmd | 0x01;
  asm_out32(PCI_COMMAND_PORT, cmd);
}

// TODO 重构循环体到新函数

void init_pci(void *addr_base) {
  u32 i, BUS, Equipment, F, ADDER, *i1;
  u8 *PCI_DATA = (void *)addr_base, *PCI_DATA1;
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
              i1 = (void *)i;
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

void pci_classcode_print(struct pci_config_space_public *pci_config_space_puclic) {
  u8 *pci_drive = (u8 *)pci_config_space_puclic - 12;
  klogd("%x %x", pci_config_space_puclic->VendorID, pci_config_space_puclic->DeviceID);
  printf("BUS:%02x ", pci_drive[1]);
  printf("EQU:%02x ", pci_drive[2]);
  printf("F:%02x ", pci_drive[3]);
  printf("IO Port:%08x ", pci_get_port_base(pci_drive[1], pci_drive[2], pci_drive[3]));
  printf("IRQ Line:%02x ", pci_get_drive_irq(pci_drive[1], pci_drive[2], pci_drive[3]));
  if (pci_config_space_puclic->BaseClass == 0x0) {
    printf("Nodefined ");
    if (pci_config_space_puclic->SubClass == 0x0)
      printf("Non-VGA-Compatible Unclassified Device\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      printf("VGA-Compatible Unclassified Device\n");
  } else if (pci_config_space_puclic->BaseClass == 0x1) {
    printf("Mass Storage Controller ");
    if (pci_config_space_puclic->SubClass == 0x0)
      printf("SCSI Bus Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      printf("IDE Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      printf("Floppy Disk Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x3)
      printf("IPI Bus Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x4)
      printf("RAID Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x5)
      printf("ATA Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x6)
      printf("Serial ATA Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x7)
      printf("Serial Attached SCSI Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x8)
      printf("Non-Volatile Memory Controller\n");
    else
      printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x2) {
    printf("Network Controller ");
    if (pci_config_space_puclic->SubClass == 0x0)
      printf("Ethernet Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      printf("Token Ring Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      printf("FDDI Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x3)
      printf("ATM Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x4)
      printf("ISDN Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x5)
      printf("WorldFip Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x6)
      printf("PICMG 2.14 Multi Computing Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x7)
      printf("Infiniband Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x8)
      printf("Fabric Controller\n");
    else
      printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x3) {
    printf("Display Controller ");
    if (pci_config_space_puclic->SubClass == 0x0)
      printf("VGA Compatible Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      printf("XGA Controller\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      printf("3D Controller (Not VGA-Compatible)\n");
    else
      printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x4) {
    printf("Multimedia Controller ");
    printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x5) {
    printf("Memory Controller ");
    printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x6) {
    printf("Bridge ");
    if (pci_config_space_puclic->SubClass == 0x0)
      printf("Host Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x1)
      printf("ISA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x2)
      printf("EISA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x3)
      printf("MCA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x4 || pci_config_space_puclic->SubClass == 0x9)
      printf("PCI-to-PCI Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x5)
      printf("PCMCIA Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x6)
      printf("NuBus Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x7)
      printf("CardBus Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0x8)
      printf("RACEway Bridge\n");
    else if (pci_config_space_puclic->SubClass == 0xA)
      printf("InfiniBand-to-PCI Host Bridge\n");
    else
      printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x7) {
    printf("Simple Communication Controller ");
    printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x8) {
    printf("Base System Peripheral ");
    printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0x9) {
    printf("Input Device Controller ");
    printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0xA) {
    printf("Docking Station ");
    printf("\n");
  } else if (pci_config_space_puclic->BaseClass == 0xB) {
    printf("Processor ");
    printf("\n");
  } else {
    printf("Unknow\n");
  }
}
