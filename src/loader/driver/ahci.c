// AHCI Controller Driver Implement

#include <dosldr.h>
#define logk printf
static u8 *cache;

static u32      ahci_bus, ahci_slot, ahci_func, port, ahci_ports_base_addr;
static u32      drive_mapping[0xff];
static u32      ports[32];
static u32      port_total = 0;
static HBA_MEM *hba_mem_address;
static void     ahci_vdisk_read(char drive, u8 *buffer, u32 number, u32 lba);
static void     ahci_vdisk_write(char drive, u8 *buffer, u32 number, u32 lba);
static int      check_type(HBA_PORT *port) {
  u32 ssts = port->ssts;

  u8 ipm = (ssts >> 8) & 0x0F;
  u8 det = ssts & 0x0F;
  // https://www.intel.com/content/dam/www/public/us/en/documents/technical-specifications/serial-ata-ahci-spec-rev1-3-1.pdf
  // 3.3.10
  if (det != HBA_PORT_DET_PRESENT) return AHCI_DEV_NULL;
  if (ipm != HBA_PORT_IPM_ACTIVE) return AHCI_DEV_NULL;

  switch (port->sig) {
  case SATA_SIG_ATAPI: return AHCI_DEV_SATAPI;
  case SATA_SIG_SEMB: return AHCI_DEV_SEMB;
  case SATA_SIG_PM: return AHCI_DEV_PM;
  default: return AHCI_DEV_SATA;
  }
}
void ahci_search_ports(HBA_MEM *abar) {
  // Search disk in implemented ports
  u32 pi = abar->pi;
  int i  = 0;
  while (i < 32) {
    if (pi & 1) {
      int dt = check_type(&abar->ports[i]);
      if (dt == AHCI_DEV_SATA) {
        logk("SATA drive found at port %d\n", i);
        port                = i;
        ports[port_total++] = i;
      } else if (dt == AHCI_DEV_SATAPI) {
        logk("SATAPI drive found at port %d\n", i);
      } else if (dt == AHCI_DEV_SEMB) {
        logk("SEMB drive found at port %d\n", i);
      } else if (dt == AHCI_DEV_PM) {
        logk("PM drive found at port %d\n", i);
      } else {
        logk("No drive found at port %d\n", i);
      }
    }

    pi >>= 1;
    i++;
  }
}
// Start command engine
void start_cmd(HBA_PORT *port) {
  // Wait until CR (bit15) is cleared
  while (port->cmd & HBA_PxCMD_CR)
    ;

  // Set FRE (bit4) and ST (bit0)
  port->cmd |= HBA_PxCMD_FRE;
  port->cmd |= HBA_PxCMD_ST;
}

// Stop command engine
void stop_cmd(HBA_PORT *port) {
  // Clear ST (bit0)
  port->cmd &= ~HBA_PxCMD_ST;

  // Clear FRE (bit4)
  port->cmd &= ~HBA_PxCMD_FRE;

  // Wait until FR (bit14), CR (bit15) are cleared
  while (1) {
    if (port->cmd & HBA_PxCMD_FR) continue;
    if (port->cmd & HBA_PxCMD_CR) continue;
    break;
  }
}

#define ATA_DEV_BUSY           0x80
#define ATA_DEV_DRQ            0x08
#define AHCI_CMD_READ_DMA_EXT  0x25
#define AHCI_CMD_WRITE_DMA_EXT 0x35
bool ahci_read(HBA_PORT *port, u32 startl, u32 starth, u32 count, u16 *buf) {
  port->is = (u32)-1; // Clear pending interrupt bits
  int spin = 0;       // Spin lock timeout counter
  int slot = find_cmdslot(port);
  if (slot == -1) return false;

  HBA_CMD_HEADER *cmdheader  = (HBA_CMD_HEADER *)port->clb;
  cmdheader                 += slot;
  cmdheader->cfl             = sizeof(FIS_REG_H2D) / sizeof(u32); // Command FIS size
  cmdheader->w               = 0;                                 // Read from device
  cmdheader->c               = 1;
  cmdheader->p               = 1;
  cmdheader->prdtl           = (u16)((count - 1) >> 4) + 1; // PRDT entries count

  HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);
  memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

  // 8K bytes (16 sectors) per PRDT
  int i;
  for (i = 0; i < cmdheader->prdtl - 1; i++) {
    cmdtbl->prdt_entry[i].dba  = (u32)buf;
    cmdtbl->prdt_entry[i].dbau = 0;
    cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1; // 8K bytes (this value should always be set to 1 less
                                              // than the actual value)
    cmdtbl->prdt_entry[i].i  = 1;
    buf                     += 4 * 1024; // 4K words
    count                   -= 16;       // 16 sectors
  }
  // Last entry
  cmdtbl->prdt_entry[i].dba = (u32)buf;
  cmdtbl->prdt_entry[i].dbc = (count << 9) - 1; // 512 bytes per sector
  cmdtbl->prdt_entry[i].i   = 1;

  // Setup command
  FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);

  cmdfis->fis_type = FIS_TYPE_REG_H2D;
  cmdfis->c        = 1; // Command
  cmdfis->command  = AHCI_CMD_READ_DMA_EXT;

  cmdfis->lba0   = (u8)startl;
  cmdfis->lba1   = (u8)(startl >> 8);
  cmdfis->lba2   = (u8)(startl >> 16);
  cmdfis->device = 1 << 6; // LBA mode

  cmdfis->lba3 = (u8)(startl >> 24);
  cmdfis->lba4 = (u8)starth;
  cmdfis->lba5 = (u8)(starth >> 8);

  cmdfis->countl = count & 0xFF;
  cmdfis->counth = (count >> 8) & 0xFF;

  // The below loop waits until the port is no longer busy before issuing a new
  // command
  while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
    spin++;
  }
  if (spin == 1000000) {
    logk("Port is hung\n");
    return false;
  }

  port->ci = 1 << slot; // Issue command

  // Wait for completion
  while (1) {
    // In some longer duration reads, it may be helpful to spin on the DPS bit
    // in the PxIS port field as well (1 << 5)
    if ((port->ci & (1 << slot)) == 0) break;
    if (port->is & HBA_PxIS_TFES) // Task file error
    {
      logk("Read disk error\n");
      return false;
    }
  }

  // Check again
  if (port->is & HBA_PxIS_TFES) {
    logk("Read disk error\n");
    return false;
  }

  return true;
}

bool ahci_identify(HBA_PORT *port, void *buf) {
  port->is = (u32)-1; // Clear pending interrupt bits
  int spin = 0;       // Spin lock timeout counter
  int slot = find_cmdslot(port);
  if (slot == -1) return false;

  HBA_CMD_HEADER *cmdheader  = (HBA_CMD_HEADER *)port->clb;
  cmdheader                 += slot;
  cmdheader->cfl             = sizeof(FIS_REG_H2D) / sizeof(u32); // Command FIS size
  cmdheader->w               = 0;                                 // Read from device
  cmdheader->prdtl           = 1;                                 // PRDT entries count
  cmdheader->c               = 1;
  HBA_CMD_TBL *cmdtbl        = (HBA_CMD_TBL *)(cmdheader->ctba);
  memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

  cmdtbl->prdt_entry[0].dba  = (u32)buf;
  cmdtbl->prdt_entry[0].dbau = 0;
  cmdtbl->prdt_entry[0].dbc  = 0x200 - 1;
  cmdtbl->prdt_entry[0].i    = 1;

  // Setup command
  FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);

  cmdfis->fis_type = FIS_TYPE_REG_H2D;
  cmdfis->c        = 1;    // Command
  cmdfis->command  = 0xec; // ATA IDENTIFY

  // The below loop waits until the port is no longer busy before issuing a new
  // command
  while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
    spin++;
  }
  if (spin == 1000000) {
    logk("Port is hung\n");
    return false;
  }

  port->ci = 1 << slot; // Issue command

  // Wait for completion
  while (1) {
    // In some longer duration reads, it may be helpful to spin on the DPS bit
    // in the PxIS port field as well (1 << 5)
    if ((port->ci & (1 << slot)) == 0) break;
    if (port->is & HBA_PxIS_TFES) // Task file error
    {
      logk("Read disk error\n");
      return false;
    }
  }

  // Check again
  if (port->is & HBA_PxIS_TFES) {
    logk("Read disk error\n");
    return false;
  }

  return true;
}

bool ahci_write(HBA_PORT *port, u32 startl, u32 starth, u32 count, u16 *buf) {
  port->is = (u32)-1; // Clear pending interrupt bits
  int spin = 0;       // Spin lock timeout counter
  int slot = find_cmdslot(port);
  if (slot == -1) return false;

  HBA_CMD_HEADER *cmdheader  = (HBA_CMD_HEADER *)port->clb;
  cmdheader                 += slot;
  cmdheader->cfl             = sizeof(FIS_REG_H2D) / sizeof(u32); // Command FIS size
  cmdheader->w               = 1;                                 // 写硬盘
  cmdheader->p               = 1;
  cmdheader->c               = 1;
  cmdheader->prdtl           = (u16)((count - 1) >> 4) + 1; // PRDT entries count

  HBA_CMD_TBL *cmdtbl = (HBA_CMD_TBL *)(cmdheader->ctba);
  memset(cmdtbl, 0, sizeof(HBA_CMD_TBL) + (cmdheader->prdtl - 1) * sizeof(HBA_PRDT_ENTRY));

  // 8K bytes (16 sectors) per PRDT
  int i;
  for (i = 0; i < cmdheader->prdtl - 1; i++) {
    cmdtbl->prdt_entry[i].dba  = (u32)buf;
    cmdtbl->prdt_entry[i].dbau = 0;
    cmdtbl->prdt_entry[i].dbc = 8 * 1024 - 1; // 8K bytes (this value should always be set to 1 less
                                              // than the actual value)
    cmdtbl->prdt_entry[i].i  = 1;
    buf                     += 4 * 1024; // 4K words
    count                   -= 16;       // 16 sectors
  }
  // Last entry
  cmdtbl->prdt_entry[i].dba = (u32)buf;
  cmdtbl->prdt_entry[i].dbc = (count << 9) - 1; // 512 bytes per sector
  cmdtbl->prdt_entry[i].i   = 1;

  // Setup command
  FIS_REG_H2D *cmdfis = (FIS_REG_H2D *)(&cmdtbl->cfis);

  cmdfis->fis_type = FIS_TYPE_REG_H2D;
  cmdfis->c        = 1; // Command
  cmdfis->command  = AHCI_CMD_WRITE_DMA_EXT;

  cmdfis->lba0   = (u8)startl;
  cmdfis->lba1   = (u8)(startl >> 8);
  cmdfis->lba2   = (u8)(startl >> 16);
  cmdfis->device = 1 << 6; // LBA mode

  cmdfis->lba3 = (u8)(startl >> 24);
  cmdfis->lba4 = (u8)starth;
  cmdfis->lba5 = (u8)(starth >> 8);

  cmdfis->countl = count & 0xFF;
  cmdfis->counth = (count >> 8) & 0xFF;

  // The below loop waits until the port is no longer busy before issuing a new
  // command
  while ((port->tfd & (ATA_DEV_BUSY | ATA_DEV_DRQ)) && spin < 1000000) {
    spin++;
  }
  if (spin == 1000000) {
    logk("Port is hung\n");
    return false;
  }

  port->ci = 1 << slot; // Issue command

  // Wait for completion
  while (1) {
    // In some longer duration reads, it may be helpful to spin on the DPS bit
    // in the PxIS port field as well (1 << 5)
    if ((port->ci & (1 << slot)) == 0) break;
    if (port->is & HBA_PxIS_TFES) // Task file error
    {
      logk("Write disk error\n");
      return false;
    }
  }

  // Check again
  if (port->is & HBA_PxIS_TFES) {
    logk("Write disk error\n");
    return false;
  }

  return true;
}
// Find a free command list slot
int find_cmdslot(HBA_PORT *port) {
  // If not set in SACT and CI, the slot is free
  u32 slots    = (port->sact | port->ci);
  int cmdslots = (hba_mem_address->cap & 0x1f00) >> 8;
  for (int i = 0; i < cmdslots; i++) {
    if ((slots & 1) == 0) return i;
    slots >>= 1;
  }
  logk("Cannot find free command list entry\n");
  return -1;
}
void page_set_attr(unsigned start, unsigned end, unsigned attr, unsigned pde);
void port_rebase(HBA_PORT *port, int portno) {
  stop_cmd(port); // Stop command engine

  // Command list offset: 1K*portno
  // Command list entry size = 32
  // Command list entry maxim count = 32
  // Command list maxim size = 32*32 = 1K per port
  port->clb  = ahci_ports_base_addr + (portno << 10);
  port->clbu = 0;
  memset((void *)(port->clb), 0, 1024);

  // FIS offset: 32K+256*portno
  // FIS entry size = 256 bytes per port
  port->fb  = ahci_ports_base_addr + (32 << 10) + (portno << 8);
  port->fbu = 0;
  memset((void *)(port->fb), 0, 256);

  // Command table offset: 40K + 8K*portno
  // Command table size = 256*32 = 8K per port
  HBA_CMD_HEADER *cmdheader = (HBA_CMD_HEADER *)(port->clb);
  for (int i = 0; i < 32; i++) {
    cmdheader[i].prdtl = 8; // 8 prdt entries per command table
                            // 256 bytes per command table, 64+16+48+16*8
    // Command table offset: 40K + 8K*portno + cmdheader_index*256
    cmdheader[i].ctba  = ahci_ports_base_addr + (40 << 10) + (portno << 13) + (i << 8);
    cmdheader[i].ctbau = 0;
    memset((void *)cmdheader[i].ctba, 0, 256);
  }

  start_cmd(port); // Start command engine
}

void ahci_init() {
  int i, j, k;
  int flag = 0;
  for (i = 0; i < 255; i++) {
    for (j = 0; j < 32; j++) {
      for (k = 0; k < 8; k++) {
        u32  p     = read_pci(i, j, k, 0x8);
        u16 *reg   = &p;        // reg[0] ---> P & R, reg[1] ---> Sub Class Class Code
        u8  *codes = &(reg[1]); // codes[0] --> Sub Class Code  codes[1] Class Code
        if (codes[1] == 0x1 && codes[0] == 0x6) {
          ahci_bus  = i;
          ahci_slot = j;
          ahci_func = k;
          flag      = 1;
          goto OK;
        }
      }
    }
  }
OK:
  if (!flag) {
    logk("Couldn't find AHCI Controller\n");
    return;
  }
  hba_mem_address = (HBA_MEM *)read_bar_n(ahci_bus, ahci_slot, ahci_func, 5);
  logk("HBA Address has been Mapped in %08x ", hba_mem_address);
  // 设置允许中断产生
  u32 conf  = pci_read_command_status(ahci_bus, ahci_slot, ahci_func);
  conf     &= 0xffff0000;
  conf     |= 0x7;
  pci_write_command_status(ahci_bus, ahci_slot, ahci_func, conf);

  // 设置HBA中 GHC控制器的 AE（AHCI Enable）位，关闭AHCI控制器的IDE仿真模式
  hba_mem_address->ghc |= (1 << 31);
  logk("AHCI Enable = %08x\n", (hba_mem_address->ghc & (1 << 31)) >> 31);

  ahci_search_ports(hba_mem_address);

  ahci_ports_base_addr = (u32)page_malloc(1048576);

  logk("AHCI port base address has been alloced in 0x%08x!\n", ahci_ports_base_addr);
  logk("The Useable Ports:");
  for (i = 0; i < port_total; i++) {
    logk("%d ", ports[i]);
    port_rebase(&(hba_mem_address->ports[ports[i]]), ports[i]);
  }
  logk("\n");

  for (i = 0; i < port_total; i++) {
    SATA_ident_t buf;
    int          a = ahci_identify(&(hba_mem_address->ports[ports[i]]), &buf);
    if (!a) {
      logk("SATA Drive %d identify error.\n");
      continue;
    }
    logk("ports %d: total sector = %d\n", ports[i], buf.lba_capacity);
    vdisk vd;
    vd.flag  = 1;
    vd.Read  = ahci_vdisk_read;
    vd.Write = ahci_vdisk_write;
    vd.size  = buf.lba_capacity * 512;
    u8 drive = register_vdisk(vd);
    logk("drive: %c\n", drive);
    drive_mapping[drive] = ports[i];
  }
}

static void ahci_vdisk_read(char drive, u8 *buffer, u32 number, u32 lba) {
  //                    logk("mapping %d\n",drive_mapping[drive]);
  ahci_read(&(hba_mem_address->ports[drive_mapping[drive]]), lba, 0, number, buffer);
}
static void ahci_vdisk_write(char drive, u8 *buffer, u32 number, u32 lba) {}