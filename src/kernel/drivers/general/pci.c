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
// true: there is an interrupt pending(handling or unhandling)
// false: there is no interrupt pending or it has been handled
bool pci_check_interrupt_status(u8 bus, u8 slot, u8 func) {
  u32 status = pci_read_command_status(bus, slot, func) >> 16; // 我们需要status寄存器
  return (status & (1 << 3)) != 0;
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

extern void *pci_addr_base;

int pci_get_device(int vendor_id, int device_id, int subsystem_id, u8 *bus, u8 *slot, u8 *func) {
  byte *pci_drive = pci_addr_base;
  for (;; pci_drive += 0x110 + 4) {
    if (pci_drive[0] != 0xff) break;
    val pci_config_space_puclic = (struct pci_config_space_public *)(pci_drive + 0x0c);
    if (vendor_id >= 0 && pci_config_space_puclic->VendorID != vendor_id) continue;
    if (device_id >= 0 && pci_config_space_puclic->DeviceID != device_id) continue;
    if (subsystem_id >= 0 && pci_config_space_puclic->SubSystemID != subsystem_id) continue;
    *bus  = pci_drive[1];
    *slot = pci_drive[2];
    *func = pci_drive[3];
    return 0;
  }
  *bus  = 0xff;
  *slot = 0xff;
  *func = 0xff;
  return -1;
}

void pci_config(u32 bus, u32 f, u32 equipment, u32 adder) {
  u32 cmd = 0;
  cmd     = 0x80000000 + (u32)adder + ((u32)f << 8) + ((u32)equipment << 11) + ((u32)bus << 16);
  asm_out32(PCI_COMMAND_PORT, cmd);
}

// TODO 重构循环体到新函数

void init_pci(void *addr_base) {
  u32 i, bus, equipment, func, addr, *i1;
  u8 *pci_dat = (void *)addr_base, *pci_dat1;
  for (bus = 0; bus < 256; bus++) {                    //查询总线
    for (equipment = 0; equipment < 32; equipment++) { //查询设备
      for (func = 0; func < 8; func++) {               //查询功能
        pci_config(bus, func, equipment, 0);
        if (asm_in32(PCI_DATA_PORT) != 0xFFFFFFFF) {
          //当前插槽有设备
          //把当前设备信息映射到PCI数据区
          int key = 1;
          while (key) {
            //此配置表为空
            pci_dat1  = pci_dat;
            *pci_dat1 = 0xFF; //表占用标志
            pci_dat1++;
            *pci_dat1 = bus; //总线号
            pci_dat1++;
            *pci_dat1 = equipment; //设备号
            pci_dat1++;
            *pci_dat1 = func; //功能号
            pci_dat1++;
            pci_dat1 = pci_dat1 + 8;
            //写入寄存器配置
            for (addr = 0; addr < 256; addr = addr + 4) {
              pci_config(bus, func, equipment, addr);
              i  = asm_in32(PCI_DATA_PORT);
              i1 = (void *)i;
              memcpy(pci_dat1, &i, 4);
              pci_dat1 = pci_dat1 + 4;
            }
            for (u8 bar_num = 0; bar_num < 6; bar_num++) {
              base_address_register bar = get_base_address_register(bus, equipment, func, bar_num);
              if (bar.address && (bar.type == input_output)) {
                pci_dat1 += 4;
                int i     = ((u32)(bar.address));
                memcpy(pci_dat1, &i, 4);
              }
            }
            pci_dat = pci_dat + 0x110 + 4;
            key     = 0;
          }
        }
      }
    }
  }
  //函数执行完PCI_DATA就是PCI设备表的结束地址
}

void pci_classcode_print(const struct pci_config_space_public *pci_config_space_puclic) {
  val pci_drive = (u8 *)pci_config_space_puclic - 12;
  klogd("%04x %04x %04x", pci_config_space_puclic->VendorID, pci_config_space_puclic->DeviceID,
        pci_config_space_puclic->SubSystemID);
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
