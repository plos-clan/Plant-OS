#include <dosldr.h>
static inline void nul(char *f, ...) {}
#define printk nul
u8 ide_read(u8 channel, u8 reg);
#define inb                     asm_in8
#define outb                    asm_out8
#define ATA_SR_BSY              0x80 // Busy
#define ATA_SR_DRDY             0x40 // Drive ready
#define ATA_SR_DF               0x20 // Drive write fault
#define ATA_SR_DSC              0x10 // Drive seek complete
#define ATA_SR_DRQ              0x08 // Data request ready
#define ATA_SR_CORR             0x04 // Corrected data
#define ATA_SR_IDX              0x02 // Index
#define ATA_SR_ERR              0x01 // Error
#define ATA_ER_BBK              0x80 // Bad block
#define ATA_ER_UNC              0x40 // Uncorrectable data
#define ATA_ER_MC               0x20 // Media changed
#define ATA_ER_IDNF             0x10 // ID mark not found
#define ATA_ER_MCR              0x08 // Media change request
#define ATA_ER_ABRT             0x04 // Command aborted
#define ATA_ER_TK0NF            0x02 // Track 0 not found
#define ATA_ER_AMNF             0x01 // No address mark
#define ATA_CMD_READ_PIO        0x20
#define ATA_CMD_READ_PIO_EXT    0x24
#define ATA_CMD_READ_DMA        0xC8
#define ATA_CMD_READ_DMA_EXT    0x25
#define ATA_CMD_WRITE_PIO       0x30
#define ATA_CMD_WRITE_PIO_EXT   0x34
#define ATA_CMD_WRITE_DMA       0xCA
#define ATA_CMD_WRITE_DMA_EXT   0x35
#define ATA_CMD_CACHE_FLUSH     0xE7
#define ATA_CMD_CACHE_FLUSH_EXT 0xEA
#define ATA_CMD_PACKET          0xA0
#define ATA_CMD_IDENTIFY_PACKET 0xA1
#define ATA_CMD_IDENTIFY        0xEC
#define ATAPI_CMD_READ          0xA8
#define ATAPI_CMD_EJECT         0x1B
#define ATA_IDENT_DEVICETYPE    0
#define ATA_IDENT_CYLINDERS     2
#define ATA_IDENT_HEADS         6
#define ATA_IDENT_SECTORS       12
#define ATA_IDENT_SERIAL        20
#define ATA_IDENT_MODEL         54
#define ATA_IDENT_CAPABILITIES  98
#define ATA_IDENT_FIELDVALID    106
#define ATA_IDENT_MAX_LBA       120
#define ATA_IDENT_COMMANDSETS   164
#define ATA_IDENT_MAX_LBA_EXT   200
#define IDE_ATA                 0x00
#define IDE_ATAPI               0x01

#define ATA_MASTER         0x00
#define ATA_SLAVE          0x01
#define ATA_REG_DATA       0x00
#define ATA_REG_ERROR      0x01
#define ATA_REG_FEATURES   0x01
#define ATA_REG_SECCOUNT0  0x02
#define ATA_REG_LBA0       0x03
#define ATA_REG_LBA1       0x04
#define ATA_REG_LBA2       0x05
#define ATA_REG_HDDEVSEL   0x06
#define ATA_REG_COMMAND    0x07
#define ATA_REG_STATUS     0x07
#define ATA_REG_SECCOUNT1  0x08
#define ATA_REG_LBA3       0x09
#define ATA_REG_LBA4       0x0A
#define ATA_REG_LBA5       0x0B
#define ATA_REG_CONTROL    0x0C
#define ATA_REG_ALTSTATUS  0x0C
#define ATA_REG_DEVADDRESS 0x0D
// Channels:
#define ATA_PRIMARY        0x00
#define ATA_SECONDARY      0x01

// Directions:
#define ATA_READ  0x00
#define ATA_WRITE 0x01
struct IDEChannelRegisters {
  u16 base;  // I/O Base.
  u16 ctrl;  // Control Base
  u16 bmide; // Bus Master IDE
  u8  nIEN;  // nIEN (No Interrupt);
} channels[2];
u8                            ide_buf[2048]    = {0};
volatile unsigned static char ide_irq_invoked  = 0;
unsigned static char          atapi_packet[12] = {0xA8, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
static int                    package[2];
struct ide_device {
  u8  Reserved;     // 0 (Empty) or 1 (This Drive really exists).
  u8  Channel;      // 0 (Primary Channel) or 1 (Secondary Channel).
  u8  Drive;        // 0 (Master Drive) or 1 (Slave Drive).
  u16 Type;         // 0: ATA, 1:ATAPI.
  u16 Signature;    // Drive Signature
  u16 Capabilities; // Features.
  u32 CommandSets;  // Command Sets Supported.
  u32 Size;         // Size in Sectors.
  u8  Model[41];    // Model in string.
} ide_devices[4];
static inline void insl(u32 port, u32 *addr, int cnt) {
  for (int i = 0; i < cnt; i++) {
    addr[i] = asm_in32(port);
  }
}
static void Read(char drive, u8 *buffer, u32 number, u32 lba) {
  ide_read_sectors(drive - 'C', number, lba, 1 * 8, (u32)buffer);
}
static void Write(char drive, u8 *buffer, u32 number, u32 lba) {
  ide_write_sectors(drive - 'C', number, lba, 1 * 8, (u32)buffer);
}
void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4) {

  ClearMaskIrq(15);
  int j, k, count = 0;
  for (int i = 0; i < 4; i++) {
    ide_devices[i].Reserved = 0;
  }
  // 1- Detect I/O Ports which interface IDE Controller:
  channels[ATA_PRIMARY].base    = (BAR0 & 0xFFFFFFFC) + 0x1F0 * (!BAR0);
  channels[ATA_PRIMARY].ctrl    = (BAR1 & 0xFFFFFFFC) + 0x3F6 * (!BAR1);
  channels[ATA_SECONDARY].base  = (BAR2 & 0xFFFFFFFC) + 0x170 * (!BAR2);
  channels[ATA_SECONDARY].ctrl  = (BAR3 & 0xFFFFFFFC) + 0x376 * (!BAR3);
  channels[ATA_PRIMARY].bmide   = (BAR4 & 0xFFFFFFFC) + 0; // Bus Master IDE
  channels[ATA_SECONDARY].bmide = (BAR4 & 0xFFFFFFFC) + 8; // Bus Master IDE
                                                           // 2- Disable IRQs:
  ide_write(ATA_PRIMARY, ATA_REG_CONTROL, 2);
  ide_write(ATA_SECONDARY, ATA_REG_CONTROL, 2);
  printk("w1\n");
  // 3- Detect ATA-ATAPI Devices:
  for (int i = 0; i < 2; i++)
    for (j = 0; j < 2; j++) {
      u8 err = 0, type = IDE_ATA, status;
      ide_devices[count].Reserved = 0; // Assuming that no drive here.

      // (I) Select Drive:
      ide_write(i, ATA_REG_HDDEVSEL, 0xA0 | (j << 4)); // Select Drive.
      // sleep(1);  // Wait 1ms for drive select to work.
      printk("I\n");
      // (II) Send ATA Identify Command:
      ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY);
      // sleep(1);  // This function should be implemented in your OS. which
      // waits
      //  for 1 ms. it is based on System Timer Device Driver.

      printk("II\n");
      // (III) Polling:
      if (ide_read(i, ATA_REG_STATUS) == 0) continue; // If Status = 0, No Device.
      printk("III\n");
      while (1) {
        printk("read ide....\n");
        status = ide_read(i, ATA_REG_STATUS);
        printk("read ok\n");
        if ((status & ATA_SR_ERR)) {
          err = 1;
          break;
        }                                                           // If Err, Device is not ATA.
        if (!(status & ATA_SR_BSY) && (status & ATA_SR_DRQ)) break; // Everything is right.
      }

      // (IV) Probe for ATAPI Devices:
      printk("IV\n");
      if (err != 0) {
        u8 cl = ide_read(i, ATA_REG_LBA1);
        u8 ch = ide_read(i, ATA_REG_LBA2);

        if (cl == 0x14 && ch == 0xEB)
          type = IDE_ATAPI;
        else if (cl == 0x69 && ch == 0x96)
          type = IDE_ATAPI;
        else
          continue; // Unknown Type (may not be a device).

        ide_write(i, ATA_REG_COMMAND, ATA_CMD_IDENTIFY_PACKET);
        // sleep(1);
      }

      // (V) Read Identification Space of the Device:
      printk("V\n");
      ide_read_buffer(i, ATA_REG_DATA, (u32)ide_buf, 128);

      // (VI) Read Device Parameters:
      printk("VI\n");
      ide_devices[count].Reserved     = 1;
      ide_devices[count].Type         = type;
      ide_devices[count].Channel      = i;
      ide_devices[count].Drive        = j;
      ide_devices[count].Signature    = *((u16 *)(ide_buf + ATA_IDENT_DEVICETYPE));
      ide_devices[count].Capabilities = *((u16 *)(ide_buf + ATA_IDENT_CAPABILITIES));
      ide_devices[count].CommandSets  = *((u32 *)(ide_buf + ATA_IDENT_COMMANDSETS));

      // (VII) Get Size:
      printk("VII\n");
      if (ide_devices[count].CommandSets & (1 << 26))
        // Device uses 48-Bit Addressing:
        ide_devices[count].Size = *((u32 *)(ide_buf + ATA_IDENT_MAX_LBA_EXT));
      else
        // Device uses CHS or 28-bit Addressing:
        ide_devices[count].Size = *((u32 *)(ide_buf + ATA_IDENT_MAX_LBA));

      // (VIII) String indicates model of device (like Western Digital HDD and
      // SONY DVD-RW...):
      printk("VIII\n");
      for (k = 0; k < 40; k += 2) {
        ide_devices[count].Model[k]     = ide_buf[ATA_IDENT_MODEL + k + 1];
        ide_devices[count].Model[k + 1] = ide_buf[ATA_IDENT_MODEL + k];
      }
      ide_devices[count].Model[40] = 0; // Terminate String.

      count++;
    }

  // 4- Print Summary:
  vdisk vd;
  for (int i = 0; i < 4; i++)
    if (ide_devices[i].Reserved == 1) {
      printk(" %d Found %s Drive %dMB - %s\n", i,
             (const char *[]){"ATA", "ATAPI"}[ide_devices[i].Type], /* Type */
             ide_devices[i].Size / 1024 / 2,                        /* Size */
             ide_devices[i].Model);
      strcpy(vd.DriveName, ide_devices[i].Model);
      if (ide_devices[i].Type == IDE_ATAPI) {
        vd.flag = 2;
      } else {
        vd.flag = 1;
      }
      vd.Read  = Read;
      vd.Write = Write;
      vd.size  = ide_devices[i].Size / 2 * 1024;
      register_vdisk(vd);
    }
}
u8 ide_read(u8 channel, u8 reg) {
  u8 result;
  if (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
  if (reg < 0x08)
    result = inb(channels[channel].base + reg - 0x00);
  else if (reg < 0x0C)
    result = inb(channels[channel].base + reg - 0x06);
  else if (reg < 0x0E)
    result = inb(channels[channel].ctrl + reg - 0x0A);
  else if (reg < 0x16)
    result = inb(channels[channel].bmide + reg - 0x0E);
  if (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
  return result;
}
void ide_write(u8 channel, u8 reg, u8 data) {
  if (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
  if (reg < 0x08)
    outb(channels[channel].base + reg - 0x00, data);
  else if (reg < 0x0C)
    outb(channels[channel].base + reg - 0x06, data);
  else if (reg < 0x0E)
    outb(channels[channel].ctrl + reg - 0x0A, data);
  else if (reg < 0x16)
    outb(channels[channel].bmide + reg - 0x0E, data);
  if (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}
void ide_read_buffer(u8 channel, u8 reg, u32 buffer, u32 quads) {
  /* WARNING: This code contains a serious bug. The inline assembly trashes ES
   * and ESP for all of the code the compiler generates between the inline
   *           assembly blocks.
   */
  if (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, 0x80 | channels[channel].nIEN);
  // asm("pushw %es; movw %ds, %ax; movw %ax, %es");
  if (reg < 0x08)
    insl(channels[channel].base + reg - 0x00, (u32 *)buffer, quads);
  else if (reg < 0x0C)
    insl(channels[channel].base + reg - 0x06, (u32 *)buffer, quads);
  else if (reg < 0x0E)
    insl(channels[channel].ctrl + reg - 0x0A, (u32 *)buffer, quads);
  else if (reg < 0x16)
    insl(channels[channel].bmide + reg - 0x0E, (u32 *)buffer, quads);
  // asm("popw %es;");
  if (reg > 0x07 && reg < 0x0C) ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN);
}
u8 ide_polling(u8 channel, u32 advanced_check) {
  // (I) Delay 400 nanosecond for BSY to be set:
  // -------------------------------------------------
  for (int i = 0; i < 4; i++)
    ide_read(channel, ATA_REG_ALTSTATUS); // Reading the Alternate Status port
                                          // wastes 100ns; loop four times.

  // (II) Wait for BSY to be cleared:
  // -------------------------------------------------
  printk("II\n");
  int a = ide_read(channel, ATA_REG_STATUS);
  while (a & ATA_SR_BSY) {
    printk("a=%d\n", a & ATA_SR_BSY); // Wait for BSY to be zero.
    a = ide_read(channel, ATA_REG_STATUS);
    // sleep(1);
  }
  printk("II OK\n");
  if (advanced_check) {
    u8 state = ide_read(channel, ATA_REG_STATUS); // Read Status Register.

    // (III) Check For Errors:
    // -------------------------------------------------
    printk("III\n");
    if (state & ATA_SR_ERR) return 2; // Error.

    // (IV) Check If Device fault:
    // -------------------------------------------------
    if (state & ATA_SR_DF) return 1; // Device Fault.

    // (V) Check DRQ:
    // -------------------------------------------------
    // BSY = 0; DF = 0; ERR = 0 so we should check for DRQ now.
    if ((state & ATA_SR_DRQ) == 0) return 3; // DRQ should be set
  }

  return 0; // No Error.
}
u8 ide_print_error(u32 drive, u8 err) {
  if (err == 0) return err;

  printf("IDE:");
  if (err == 1) {
    printf("- Device Fault\n     ");
    err = 19;
  } else if (err == 2) {
    u8 st = ide_read(ide_devices[drive].Channel, ATA_REG_ERROR);
    if (st & ATA_ER_AMNF) {
      printf("- No Address Mark Found\n     ");
      err = 7;
    }
    if (st & ATA_ER_TK0NF) {
      printf("- No Media or Media Error\n     ");
      err = 3;
    }
    if (st & ATA_ER_ABRT) {
      printf("- Command Aborted\n     ");
      err = 20;
    }
    if (st & ATA_ER_MCR) {
      printf("- No Media or Media Error\n     ");
      err = 3;
    }
    if (st & ATA_ER_IDNF) {
      printf("- ID mark not Found\n     ");
      err = 21;
    }
    if (st & ATA_ER_MC) {
      printf("- No Media or Media Error\n     ");
      err = 3;
    }
    if (st & ATA_ER_UNC) {
      printf("- Uncorrectable Data Error\n     ");
      err = 22;
    }
    if (st & ATA_ER_BBK) {
      printf("- Bad Sectors\n     ");
      err = 13;
    }
  } else if (err == 3) {
    printf("- Reads Nothing\n     ");
    err = 23;
  } else if (err == 4) {
    printf("- Write Protected\n     ");
    err = 8;
  }
  printf(
      "- [%s %s] %s\n",
      (const char *[]){"Primary", "Secondary"}[ide_devices[drive].Channel], // Use the channel as an
                                                                            // index into the array
      (const char *[]){"Master",
                       "Slave"}[ide_devices[drive].Drive], // Same as above, using the drive
      ide_devices[drive].Model);

  return err;
}
u8 ide_ata_access(u8 direction, u8 drive, u32 lba, u8 numsects, u16 selector, u32 edi) {
  u8  lba_mode /* 0: CHS, 1:LBA28, 2: LBA48 */, dma /* 0: No DMA, 1: DMA */, cmd;
  u8  lba_io[6];
  u32 channel  = ide_devices[drive].Channel; // Read the Channel.
  u32 slavebit = ide_devices[drive].Drive;   // Read the Drive [Master/Slave]
  u32 bus      = channels[channel].base;     // Bus Base, like 0x1F0 which is also data port.
  u32 words    = 256; // Almost every ATA drive has a sector-size of 512-byte.
  u16 cyl, i;
  u8  head, sect, err;
  ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = (ide_irq_invoked = 0x0) + 0x02);
  // (I) Select one from LBA28, LBA48 or CHS;
  printk("I %02x\n", channels[channel].nIEN);
  if (lba >= 0x10000000) { // Sure Drive should support LBA in this case, or
                           // you are giving a wrong LBA.
    // LBA48:
    lba_mode  = 2;
    lba_io[0] = (lba & 0x000000FF) >> 0;
    lba_io[1] = (lba & 0x0000FF00) >> 8;
    lba_io[2] = (lba & 0x00FF0000) >> 16;
    lba_io[3] = (lba & 0xFF000000) >> 24;
    lba_io[4] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    lba_io[5] = 0; // LBA28 is integer, so 32-bits are enough to access 2TB.
    head      = 0; // Lower 4-bits of HDDEVSEL are not used here.
  } else if (ide_devices[drive].Capabilities & 0x200) { // Drive supports LBA?
    // LBA28:
    lba_mode  = 1;
    lba_io[0] = (lba & 0x00000FF) >> 0;
    lba_io[1] = (lba & 0x000FF00) >> 8;
    lba_io[2] = (lba & 0x0FF0000) >> 16;
    lba_io[3] = 0; // These Registers are not used here.
    lba_io[4] = 0; // These Registers are not used here.
    lba_io[5] = 0; // These Registers are not used here.
    head      = (lba & 0xF000000) >> 24;
  } else {
    // CHS:
    lba_mode  = 0;
    sect      = (lba % 63) + 1;
    cyl       = (lba + 1 - sect) / (16 * 63);
    lba_io[0] = sect;
    lba_io[1] = (cyl >> 0) & 0xFF;
    lba_io[2] = (cyl >> 8) & 0xFF;
    lba_io[3] = 0;
    lba_io[4] = 0;
    lba_io[5] = 0;
    head = (lba + 1 - sect) % (16 * 63) / (63); // Head number is written to HDDEVSEL lower 4-bits.
  }
  // (II) See if drive supports DMA or not;
  printk("II\n");
  dma = 0; // We don't support DMA
           // (III) Wait if the drive is busy;
  printk("III\n");
  while (ide_read(channel, ATA_REG_STATUS) & ATA_SR_BSY) {} // Wait if busy.
  // (IV) Select Drive from the controller;
  printk("IV\n");
  if (lba_mode == 0)
    ide_write(channel, ATA_REG_HDDEVSEL,
              0xA0 | (slavebit << 4) | head); // Drive & CHS.
  else
    ide_write(channel, ATA_REG_HDDEVSEL,
              0xE0 | (slavebit << 4) | head); // Drive & LBA
                                              // (V) Write Parameters;
  if (lba_mode == 2) {
    ide_write(channel, ATA_REG_SECCOUNT1, 0);
    ide_write(channel, ATA_REG_LBA3, lba_io[3]);
    ide_write(channel, ATA_REG_LBA4, lba_io[4]);
    ide_write(channel, ATA_REG_LBA5, lba_io[5]);
  }
  ide_write(channel, ATA_REG_SECCOUNT0, numsects);
  ide_write(channel, ATA_REG_LBA0, lba_io[0]);
  ide_write(channel, ATA_REG_LBA1, lba_io[1]);
  ide_write(channel, ATA_REG_LBA2, lba_io[2]);
  if (lba_mode == 0 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
  if (lba_mode == 1 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO;
  if (lba_mode == 2 && dma == 0 && direction == 0) cmd = ATA_CMD_READ_PIO_EXT;
  if (lba_mode == 0 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
  if (lba_mode == 1 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA;
  if (lba_mode == 2 && dma == 1 && direction == 0) cmd = ATA_CMD_READ_DMA_EXT;
  if (lba_mode == 0 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
  if (lba_mode == 1 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO;
  if (lba_mode == 2 && dma == 0 && direction == 1) cmd = ATA_CMD_WRITE_PIO_EXT;
  if (lba_mode == 0 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
  if (lba_mode == 1 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA;
  if (lba_mode == 2 && dma == 1 && direction == 1) cmd = ATA_CMD_WRITE_DMA_EXT;
  ide_write(channel, ATA_REG_COMMAND, cmd); // Send the Command.
  printk("IV1\n");
  if (dma)
    if (direction == 0)
      ;
    // DMA Read.
    else
      ;
  // DMA Write.
  else if (direction == 0) {
    // PIO Read.
    u16 *word_ = (u16 *)edi;
    for (i = 0; i < numsects; i++) {
      printk("read %d\n", i);
      if (err = ide_polling(channel, 1)) return err; // Polling, set error and exit if there is.

      printk("words=%d bus=%d\n", words, bus);
      for (int h = 0; h < words; h++) {
        u16 a                = asm_in16(bus);
        word_[i * words + h] = a;
      }
    }
  } else {
    // PIO Write.

    u16 *word_ = (u16 *)edi;
    for (i = 0; i < numsects; i++) {
      printk("write %d\n", i);
      ide_polling(channel, 0); // Polling.
      // asm("pushw %ds");
      // asm("mov %%ax, %%ds" ::"a"(selector));
      // asm("rep outsw" ::"c"(words), "d"(bus), "S"(edi));  // Send Data
      // asm("popw %ds");
      for (int h = 0; h < words; h++) {
        asm_out16(bus, word_[i * words + h]);
      }
    }
    ide_write(
        channel, ATA_REG_COMMAND,
        (char[]){ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH, ATA_CMD_CACHE_FLUSH_EXT}[lba_mode]);
    ide_polling(channel, 0); // Polling.
  }

  return 0; // Easy, isn't it?
}
void ide_wait_irq() {
  while (!ide_irq_invoked)
    ;
  ide_irq_invoked = 0;
}
void ide_irq() {
  printk("ide irq.\n");
  ide_irq_invoked = 1;
}
u8 ide_atapi_read(u8 drive, u32 lba, u8 numsects, u16 selector, u32 edi) {
  printk("cdrom read.\n");
  u32 channel  = ide_devices[drive].Channel;
  u32 slavebit = ide_devices[drive].Drive;
  u32 bus      = channels[channel].base;
  u32 words    = 1024; // Sector Size. ATAPI drives have a sector size of 2048 bytes.
  u8  err;
  int i;

  ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0);
  // (I): Setup SCSI Packet:
  // ------------------------------------------------------------------
  printk("I\n");
  atapi_packet[0]  = ATAPI_CMD_READ;
  atapi_packet[1]  = 0x0;
  atapi_packet[2]  = (lba >> 24) & 0xFF;
  atapi_packet[3]  = (lba >> 16) & 0xFF;
  atapi_packet[4]  = (lba >> 8) & 0xFF;
  atapi_packet[5]  = (lba >> 0) & 0xFF;
  atapi_packet[6]  = 0x0;
  atapi_packet[7]  = 0x0;
  atapi_packet[8]  = 0x0;
  atapi_packet[9]  = numsects;
  atapi_packet[10] = 0x0;
  atapi_packet[11] = 0x0; // (II): Select the drive:
  // ------------------------------------------------------------------
  ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4);
  printk("II\n");
  // (III): Delay 400 nanoseconds for select to complete:
  for (int i = 0; i < 4000; i++)
    ;
  printk("III\n");
  // ------------------------------------------------------------------
  printk("IV\n");
  for (int i = 0; i < 4; i++)
    ide_read(channel,
             ATA_REG_ALTSTATUS); // Reading the Alternate Status port wastes
                                 // 100ns. (IV): Inform the Controller that we
                                 // use PIO mode:
  // ------------------------------------------------------------------
  ide_write(channel, ATA_REG_FEATURES,
            0); // PIO mode.
                // (V): Tell the Controller the size of buffer:
  // ------------------------------------------------------------------
  printk("V\n");
  ide_write(channel, ATA_REG_LBA1,
            (words * 2) & 0xFF); // Lower Byte of Sector Size.
  ide_write(channel, ATA_REG_LBA2,
            (words * 2) >> 8); // Upper Byte of Sector Size.
                               // (VI): Send the Packet Command:
  // ------------------------------------------------------------------
  ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET); // Send the Command.

  // (VII): Waiting for the driver to finish or return an error code:
  // ------------------------------------------------------------------
  if (err = ide_polling(channel, 1)) return err; // Polling and return if error.

  // (VIII): Sending the packet data:
  // ------------------------------------------------------------------
  printk("VIII\n");
  u16 *_atapi_packet = (u16 *)atapi_packet;
  for (int i = 0; i < 6; i++) {
    asm_out16(bus, _atapi_packet[i]);
  }
  // (IX): Receiving Data:
  // ------------------------------------------------------------------
  printk("IX\n");
  u16 *_word = (u16 *)edi;
  for (i = 0; i < numsects; i++) {
    ide_wait_irq();                                // Wait for an IRQ.
    if (err = ide_polling(channel, 1)) return err; // Polling and return if error.
    printk("words = %d\n", words);
    for (int h = 0; h < words; h++) {
      u16 a                = asm_in16(bus);
      _word[i * words + h] = a;
    }
  }
  // (X): Waiting for an IRQ:
  // ------------------------------------------------------------------
  ide_wait_irq();

  // (XI): Waiting for BSY & DRQ to clear:
  // ------------------------------------------------------------------
  while (ide_read(channel, ATA_REG_STATUS) & (ATA_SR_BSY | ATA_SR_DRQ))
    ;

  return 0; // Easy, ... Isn't it?
}
void ide_read_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi) {
  // 1: Check if the drive presents:
  // ==================================
  if (drive > 3 || ide_devices[drive].Reserved == 0) package[0] = 0x1; // Drive Not Found!

  // 2: Check if inputs are valid:
  // ==================================
  else if (((lba + numsects) > ide_devices[drive].Size) && (ide_devices[drive].Type == IDE_ATA))
    package[0] = 0x2; // Seeking to invalid position.

  // 3: Read in PIO Mode through Polling & IRQs:
  // ============================================
  else {
    u8 err;
    if (ide_devices[drive].Type == IDE_ATA) {
      printk("Will Read.\n");
      err = ide_ata_access(ATA_READ, drive, lba, numsects, es, edi);

    } else if (ide_devices[drive].Type == IDE_ATAPI)
      for (int i = 0; i < numsects; i++)
        err = ide_atapi_read(drive, lba + i, 1, es, edi + (i * 2048));
    package[0] = ide_print_error(drive, err);
  }
}
// package[0] is an entry of an array. It contains the Error Code.
void ide_write_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi) {
  // 1: Check if the drive presents:
  // ==================================
  if (drive > 3 || ide_devices[drive].Reserved == 0) package[0] = 0x1; // Drive Not Found!
  // 2: Check if inputs are valid:
  // ==================================
  else if (((lba + numsects) > ide_devices[drive].Size) && (ide_devices[drive].Type == IDE_ATA))
    package[0] = 0x2; // Seeking to invalid position.
  // 3: Read in PIO Mode through Polling & IRQs:
  // ============================================
  else {
    u8 err;
    if (ide_devices[drive].Type == IDE_ATA)
      err = ide_ata_access(ATA_WRITE, drive, lba, numsects, es, edi);
    else if (ide_devices[drive].Type == IDE_ATAPI)
      err = 4; // Write-Protected.
    package[0] = ide_print_error(drive, err);
  }
}
void ide_atapi_eject(u8 drive) {
  u32 channel     = ide_devices[drive].Channel;
  u32 slavebit    = ide_devices[drive].Drive;
  u32 bus         = channels[channel].base;
  u32 words       = 2048 / 2; // Sector Size in Words.
  u8  err         = 0;
  ide_irq_invoked = 0;

  // 1: Check if the drive presents:
  // ==================================
  if (drive > 3 || ide_devices[drive].Reserved == 0) package[0] = 0x1; // Drive Not Found!
  // 2: Check if drive isn't ATAPI:
  // ==================================
  else if (ide_devices[drive].Type == IDE_ATA)
    package[0] = 20; // Command Aborted.
  // 3: Eject ATAPI Driver:
  // ============================================
  else {
    // Enable IRQs:
    ide_write(channel, ATA_REG_CONTROL, channels[channel].nIEN = ide_irq_invoked = 0x0);

    // (I): Setup SCSI Packet:
    // ------------------------------------------------------------------
    atapi_packet[0]  = ATAPI_CMD_EJECT;
    atapi_packet[1]  = 0x00;
    atapi_packet[2]  = 0x00;
    atapi_packet[3]  = 0x00;
    atapi_packet[4]  = 0x02;
    atapi_packet[5]  = 0x00;
    atapi_packet[6]  = 0x00;
    atapi_packet[7]  = 0x00;
    atapi_packet[8]  = 0x00;
    atapi_packet[9]  = 0x00;
    atapi_packet[10] = 0x00;
    atapi_packet[11] = 0x00;

    // (II): Select the Drive:
    // ------------------------------------------------------------------
    ide_write(channel, ATA_REG_HDDEVSEL, slavebit << 4);

    // (III): Delay 400 nanosecond for select to complete:
    // ------------------------------------------------------------------
    for (int i = 0; i < 4; i++)
      ide_read(channel,
               ATA_REG_ALTSTATUS); // Reading Alternate Status Port wastes 100ns.

    // (IV): Send the Packet Command:
    // ------------------------------------------------------------------
    ide_write(channel, ATA_REG_COMMAND, ATA_CMD_PACKET); // Send the Command.

    // (V): Waiting for the driver to finish or invoke an error:
    // ------------------------------------------------------------------
    err = ide_polling(channel, 1); // Polling and stop if error.

    // (VI): Sending the packet data:
    // ------------------------------------------------------------------

    asm("rep   outsw" ::"c"(6), "d"(bus),
        "S"(atapi_packet));        // Send Packet Data
    ide_wait_irq();                // Wait for an IRQ.
    err = ide_polling(channel, 1); // Polling and get error code.
    if (err == 3) err = 0;         // DRQ is not needed here.

    package[0] = ide_print_error(drive, err); // Return;
  }
}