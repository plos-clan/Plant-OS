#pragma once
#include <define.h>
#include <type.h>
#define SATA_SIG_ATA   0x00000101 // SATA drive
#define SATA_SIG_ATAPI 0xEB140101 // SATAPI drive
#define SATA_SIG_SEMB  0xC33C0101 // Enclosure management bridge
#define SATA_SIG_PM    0x96690101 // Port multiplier
#define HBA_PxCMD_ST   0x0001
#define HBA_PxCMD_FRE  0x0010
#define HBA_PxCMD_FR   0x4000
#define HBA_PxCMD_CR   0x8000
#define HBA_PxIS_TFES  (1 << 30) /* TFES - Task File Error Status */

#define AHCI_DEV_NULL   0
#define AHCI_DEV_SATA   1
#define AHCI_DEV_SEMB   2
#define AHCI_DEV_PM     3
#define AHCI_DEV_SATAPI 4

#define HBA_PORT_IPM_ACTIVE  1
#define HBA_PORT_DET_PRESENT 3
typedef enum {
  FIS_TYPE_REG_H2D   = 0x27, // Register FIS - host to device
  FIS_TYPE_REG_D2H   = 0x34, // Register FIS - device to host
  FIS_TYPE_DMA_ACT   = 0x39, // DMA activate FIS - device to host
  FIS_TYPE_DMA_SETUP = 0x41, // DMA setup FIS - bidirectional
  FIS_TYPE_DATA      = 0x46, // Data FIS - bidirectional
  FIS_TYPE_BIST      = 0x58, // BIST activate FIS - bidirectional
  FIS_TYPE_PIO_SETUP = 0x5F, // PIO setup FIS - device to host
  FIS_TYPE_DEV_BITS  = 0xA1, // Set device bits FIS - device to host
} FIS_TYPE;
typedef volatile struct tagHBA_PORT {
  u32 clb;       // 0x00, command list base address, 1K-byte aligned
  u32 clbu;      // 0x04, command list base address upper 32 bits
  u32 fb;        // 0x08, FIS base address, 256-byte aligned
  u32 fbu;       // 0x0C, FIS base address upper 32 bits
  u32 is;        // 0x10, interrupt status
  u32 ie;        // 0x14, interrupt enable
  u32 cmd;       // 0x18, command and status
  u32 rsv0;      // 0x1C, Reserved
  u32 tfd;       // 0x20, task file data
  u32 sig;       // 0x24, signature
  u32 ssts;      // 0x28, SATA status (SCR0:SStatus)
  u32 sctl;      // 0x2C, SATA control (SCR2:SControl)
  u32 serr;      // 0x30, SATA error (SCR1:SError)
  u32 sact;      // 0x34, SATA active (SCR3:SActive)
  u32 ci;        // 0x38, command issue
  u32 sntf;      // 0x3C, SATA notification (SCR4:SNotification)
  u32 fbs;       // 0x40, FIS-based switch control
  u32 rsv1[11];  // 0x44 ~ 0x6F, Reserved
  u32 vendor[4]; // 0x70 ~ 0x7F, vendor specific
} HBA_PORT;
typedef volatile struct tagHBA_MEM {
  // 0x00 - 0x2B, Generic Host Control
  u32 cap;     // 0x00, Host capability
  u32 ghc;     // 0x04, Global host control
  u32 is;      // 0x08, Interrupt status
  u32 pi;      // 0x0C, Port implemented
  u32 vs;      // 0x10, Version
  u32 ccc_ctl; // 0x14, Command completion coalescing control
  u32 ccc_pts; // 0x18, Command completion coalescing ports
  u32 em_loc;  // 0x1C, Enclosure management location
  u32 em_ctl;  // 0x20, Enclosure management control
  u32 cap2;    // 0x24, Host capabilities extended
  u32 bohc;    // 0x28, BIOS/OS handoff control and status

  // 0x2C - 0x9F, Reserved
  u8 rsv[0xA0 - 0x2C];

  // 0xA0 - 0xFF, Vendor specific registers
  u8 vendor[0x100 - 0xA0];

  // 0x100 - 0x10FF, Port control registers
  HBA_PORT ports[1]; // 1 ~ 32
} HBA_MEM;
typedef struct tagHBA_CMD_HEADER {
  // DW0
  u8 cfl : 5; // Command FIS length in DWORDS, 2 ~ 16
  u8 a   : 1; // ATAPI
  u8 w   : 1; // Write, 1: H2D, 0: D2H
  u8 p   : 1; // Prefetchable

  u8 r    : 1; // Reset
  u8 b    : 1; // BIST
  u8 c    : 1; // Clear busy upon R_OK
  u8 rsv0 : 1; // Reserved
  u8 pmp  : 4; // Port multiplier port

  u16 prdtl; // Physical region descriptor table length in entries

  // DW1
  volatile u32 prdbc; // Physical region descriptor byte count transferred

  // DW2, 3
  u32 ctba;  // Command table descriptor base address
  u32 ctbau; // Command table descriptor base address upper 32 bits

  // DW4 - 7
  u32 rsv1[4]; // Reserved
} HBA_CMD_HEADER;
typedef struct tagFIS_REG_H2D {
  // DWORD 0
  u8 fis_type; // FIS_TYPE_REG_H2D

  u8 pmport : 4; // Port multiplier
  u8 rsv0   : 3; // Reserved
  u8 c      : 1; // 1: Command, 0: Control

  u8 command;  // Command register
  u8 featurel; // Feature register, 7:0

  // DWORD 1
  u8 lba0;   // LBA low register, 7:0
  u8 lba1;   // LBA mid register, 15:8
  u8 lba2;   // LBA high register, 23:16
  u8 device; // Device register

  // DWORD 2
  u8 lba3;     // LBA register, 31:24
  u8 lba4;     // LBA register, 39:32
  u8 lba5;     // LBA register, 47:40
  u8 featureh; // Feature register, 15:8

  // DWORD 3
  u8 countl;  // Count register, 7:0
  u8 counth;  // Count register, 15:8
  u8 icc;     // Isochronous command completion
  u8 control; // Control register

  // DWORD 4
  u8 rsv1[4]; // Reserved
} FIS_REG_H2D;
typedef struct tagFIS_REG_D2H {
  // DWORD 0
  u8 fis_type; // FIS_TYPE_REG_D2H

  u8 pmport : 4; // Port multiplier
  u8 rsv0   : 2; // Reserved
  u8 i      : 1; // Interrupt bit
  u8 rsv1   : 1; // Reserved

  u8 status; // Status register
  u8 error;  // Error register

  // DWORD 1
  u8 lba0;   // LBA low register, 7:0
  u8 lba1;   // LBA mid register, 15:8
  u8 lba2;   // LBA high register, 23:16
  u8 device; // Device register

  // DWORD 2
  u8 lba3; // LBA register, 31:24
  u8 lba4; // LBA register, 39:32
  u8 lba5; // LBA register, 47:40
  u8 rsv2; // Reserved

  // DWORD 3
  u8 countl;  // Count register, 7:0
  u8 counth;  // Count register, 15:8
  u8 rsv3[2]; // Reserved

  // DWORD 4
  u8 rsv4[4]; // Reserved
} FIS_REG_D2H;
typedef struct tagFIS_DATA {
  // DWORD 0
  u8 fis_type; // FIS_TYPE_DATA

  u8 pmport : 4; // Port multiplier
  u8 rsv0   : 4; // Reserved

  u8 rsv1[2]; // Reserved

  // DWORD 1 ~ N
  u32 data[1]; // Payload
} FIS_DATA;
typedef struct tagFIS_PIO_SETUP {
  // DWORD 0
  u8 fis_type; // FIS_TYPE_PIO_SETUP

  u8 pmport : 4; // Port multiplier
  u8 rsv0   : 1; // Reserved
  u8 d      : 1; // Data transfer direction, 1 - device to host
  u8 i      : 1; // Interrupt bit
  u8 rsv1   : 1;

  u8 status; // Status register
  u8 error;  // Error register

  // DWORD 1
  u8 lba0;   // LBA low register, 7:0
  u8 lba1;   // LBA mid register, 15:8
  u8 lba2;   // LBA high register, 23:16
  u8 device; // Device register

  // DWORD 2
  u8 lba3; // LBA register, 31:24
  u8 lba4; // LBA register, 39:32
  u8 lba5; // LBA register, 47:40
  u8 rsv2; // Reserved

  // DWORD 3
  u8 countl;   // Count register, 7:0
  u8 counth;   // Count register, 15:8
  u8 rsv3;     // Reserved
  u8 e_status; // New value of status register

  // DWORD 4
  u16 tc;      // Transfer count
  u8  rsv4[2]; // Reserved
} FIS_PIO_SETUP;
typedef struct tagFIS_DMA_SETUP {
  // DWORD 0
  u8 fis_type; // FIS_TYPE_DMA_SETUP

  u8 pmport : 4; // Port multiplier
  u8 rsv0   : 1; // Reserved
  u8 d      : 1; // Data transfer direction, 1 - device to host
  u8 i      : 1; // Interrupt bit
  u8 a      : 1; // Auto-activate. Specifies if DMA Activate FIS is needed

  u8 rsved[2]; // Reserved

  // DWORD 1&2

  u64 DMAbufferID; // DMA Buffer Identifier. Used to Identify DMA buffer in
                   // host memory. SATA Spec says host specific and not in
                   // Spec. Trying AHCI spec might work.

  // DWORD 3
  u32 rsvd; // More reserved

  // DWORD 4
  u32 DMAbufOffset; // Byte offset into buffer. First 2 bits must be 0

  // DWORD 5
  u32 TransferCount; // Number of bytes to transfer. Bit 0 must be 0

  // DWORD 6
  u32 resvd; // Reserved

} FIS_DMA_SETUP;
typedef struct tagHBA_PRDT_ENTRY {
  u32 dba;  // Data base address
  u32 dbau; // Data base address upper 32 bits
  u32 rsv0; // Reserved

  // DW3
  u32 dbc  : 22; // Byte count, 4M max
  u32 rsv1 : 9;  // Reserved
  u32 i    : 1;  // Interrupt on completion
} HBA_PRDT_ENTRY;
typedef struct tagHBA_CMD_TBL {
  // 0x00
  u8 cfis[64]; // Command FIS

  // 0x40
  u8 acmd[16]; // ATAPI command, 12 or 16 bytes

  // 0x50
  u8 rsv[48]; // Reserved

  // 0x80
  HBA_PRDT_ENTRY
  prdt_entry[1]; // Physical region descriptor table entries, 0 ~ 65535
} HBA_CMD_TBL;
typedef struct SATA_Ident {
  u16 config;        /* lots of obsolete bit flags */
  u16 cyls;          /* obsolete */
  u16 reserved2;     /* special config */
  u16 heads;         /* "physical" heads */
  u16 track_bytes;   /* unformatted bytes per track */
  u16 sector_bytes;  /* unformatted bytes per sector */
  u16 sectors;       /* "physical" sectors per track */
  u16 vendor0;       /* vendor unique */
  u16 vendor1;       /* vendor unique */
  u16 vendor2;       /* vendor unique */
  u8  serial_no[20]; /* 0 = not_specified */
  u16 buf_type;
  u16 buf_size;             /* 512 byte increments; 0 = not_specified */
  u16 ecc_bytes;            /* for r/w long cmds; 0 = not_specified */
  u8  fw_rev[8];            /* 0 = not_specified */
  u8  model[40];            /* 0 = not_specified */
  u16 multi_count;          /* Multiple Count */
  u16 dword_io;             /* 0=not_implemented; 1=implemented */
  u16 capability1;          /* vendor unique */
  u16 capability2;          /* bits 0:DMA 1:LBA 2:IORDYsw 3:IORDYsup word: 50 */
  u8  vendor5;              /* vendor unique */
  u8  tPIO;                 /* 0=slow, 1=medium, 2=fast */
  u8  vendor6;              /* vendor unique */
  u8  tDMA;                 /* 0=slow, 1=medium, 2=fast */
  u16 field_valid;          /* bits 0:cur_ok 1:eide_ok */
  u16 cur_cyls;             /* logical cylinders */
  u16 cur_heads;            /* logical heads word 55*/
  u16 cur_sectors;          /* logical sectors per track */
  u16 cur_capacity0;        /* logical total sectors on drive */
  u16 cur_capacity1;        /*  (2 words, misaligned int)     */
  u8  multsect;             /* current multiple sector count */
  u8  multsect_valid;       /* when (bit0==1) multsect is ok */
  u32 lba_capacity;         /* total number of sectors */
  u16 dma_1word;            /* single-word dma info */
  u16 dma_mword;            /* multiple-word dma info */
  u16 eide_pio_modes;       /* bits 0:mode3 1:mode4 */
  u16 eide_dma_min;         /* min mword dma cycle time (ns) */
  u16 eide_dma_time;        /* recommended mword dma cycle time (ns) */
  u16 eide_pio;             /* min cycle time (ns), no IORDY  */
  u16 eide_pio_iordy;       /* min cycle time (ns), with IORDY */
  u16 words69_70[2];        /* reserved words 69-70 */
  u16 words71_74[4];        /* reserved words 71-74 */
  u16 queue_depth;          /*  */
  u16 sata_capability;      /*  SATA Capabilities word 76*/
  u16 sata_additional;      /*  Additional Capabilities */
  u16 sata_supported;       /* SATA Features supported  */
  u16 features_enabled;     /* SATA features enabled */
  u16 major_rev_num;        /*  Major rev number word 80 */
  u16 minor_rev_num;        /*  */
  u16 command_set_1;        /* bits 0:Smart 1:Security 2:Removable 3:PM */
  u16 command_set_2;        /* bits 14:Smart Enabled 13:0 zero */
  u16 cfsse;                /* command set-feature supported extensions */
  u16 cfs_enable_1;         /* command set-feature enabled */
  u16 cfs_enable_2;         /* command set-feature enabled */
  u16 csf_default;          /* command set-feature default */
  u16 dma_ultra;            /*  */
  u16 word89;               /* reserved (word 89) */
  u16 word90;               /* reserved (word 90) */
  u16 CurAPMvalues;         /* current APM values */
  u16 word92;               /* reserved (word 92) */
  u16 comreset;             /* should be cleared to 0 */
  u16 accoustic;            /*  accoustic management */
  u16 min_req_sz;           /* Stream minimum required size */
  u16 transfer_time_dma;    /* Streaming Transfer Time-DMA */
  u16 access_latency;       /* Streaming access latency-DMA & PIO WORD 97*/
  u32 perf_granularity;     /* Streaming performance granularity */
  u32 total_usr_sectors[2]; /* Total number of user addressable sectors */
  u16 transfer_time_pio;    /* Streaming Transfer time PIO */
  u16 reserved105;          /* Word 105 */
  u16 sector_sz;            /* Puysical Sector size / Logical sector size */
  u16 inter_seek_delay;     /* In microseconds */
  u16 words108_116[9];      /*  */
  u32 words_per_sector;     /* words per logical sectors */
  u16 supported_settings;   /* continued from words 82-84 */
  u16 command_set_3;        /* continued from words 85-87 */
  u16 words121_126[6];      /* reserved words 121-126 */
  u16 word127;              /* reserved (word 127) */
  u16 security_status;      /* device lock function
                                      * 15:9   reserved
                                      * 8   security level 1:max 0:high
                                      * 7:6   reserved
                                      * 5   enhanced erase
                                      * 4   expire
                                      * 3   frozen
                                      * 2   locked
                                      * 1   en/disabled
                                      * 0   capability
                                      */
  u16 csfo;                 /* current set features options
                                      * 15:4   reserved
                                      * 3   auto reassign
                                      * 2   reverting
                                      * 1   read-look-ahead
                                      * 0   write cache
                                      */
  u16 words130_155[26];     /* reserved vendor words 130-155 */
  u16 word156;
  u16 words157_159[3];     /* reserved vendor words 157-159 */
  u16 cfa;                 /* CFA Power mode 1 */
  u16 words161_175[15];    /* Reserved */
  u8  media_serial[60];    /* words 176-205 Current Media serial number */
  u16 sct_cmd_transport;   /* SCT Command Transport */
  u16 words207_208[2];     /* reserved */
  u16 block_align;         /* Alignement of logical blocks in larger physical blocks */
  u32 WRV_sec_count;       /* Write-Read-Verify sector count mode 3 only */
  u32 verf_sec_count;      /* Verify Sector count mode 2 only */
  u16 nv_cache_capability; /* NV Cache capabilities */
  u16 nv_cache_sz;         /* NV Cache size in logical blocks */
  u16 nv_cache_sz2;        /* NV Cache size in logical blocks */
  u16 rotation_rate;       /* Nominal media rotation rate */
  u16 reserved218;         /*  */
  u16 nv_cache_options;    /* NV Cache options */
  u16 words220_221[2];     /* reserved */
  u16 transport_major_rev; /*  */
  u16 transport_minor_rev; /*  */
  u16 words224_233[10];    /* Reserved */
  u16 min_dwnload_blocks;  /* Minimum number of 512byte units per
                             DOWNLOAD MICROCODE  command for mode 03h */
  u16 max_dwnload_blocks;  /* Maximum number of 512byte units per
                             DOWNLOAD MICROCODE  command for mode 03h */
  u16 words236_254[19];    /* Reserved */
  u16 integrity;           /* Cheksum, Signature */
} SATA_ident_t;
int  find_cmdslot(HBA_PORT *port);
void ahci_init();
