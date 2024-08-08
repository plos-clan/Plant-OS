/*-----------------------------------------------------------------------*/
/* Low level disk I/O module SKELETON for FatFs     (C)ChaN, 2019        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control modules to the FatFs module with a defined API.       */
/*-----------------------------------------------------------------------*/

#include "diskio.h" /* Declarations of disk functions */
#include "ff.h"     /* Obtains integer types */
#include <kernel/vdisk.h>

/*-----------------------------------------------------------------------*/
/* Get Drive Status                                                      */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status(byte pdrv /* Physical drive nmuber to identify the drive */
) {
  DSTATUS stat = STA_NOINIT;
  int     result;
  pdrv += 0x41;
  if (have_vdisk(pdrv)) stat &= ~STA_NOINIT;
  return stat;
}

/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize(byte pdrv /* Physical drive nmuber to identify the drive */
) {
  DSTATUS stat = STA_NOINIT;
  int     result;
  pdrv += 0x41;
  if (have_vdisk(pdrv)) stat &= ~STA_NOINIT;
  return stat;
}

/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(byte  pdrv,   /* Physical drive nmuber to identify the drive */
                  byte *buff,   /* Data buffer to store read data */
                  LBA_t sector, /* Start sector in LBA */
                  uint  count   /* Number of sectors to read */
) {
  DRESULT res  = RES_PARERR;
  pdrv        += 0x41;
  if (!have_vdisk(pdrv)) return RES_PARERR;
  Disk_Read(sector, count, buff, pdrv);
  res = RES_OK;
  return res;
}

/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if FF_FS_READONLY == 0

DRESULT disk_write(byte        pdrv,   /* Physical drive nmuber to identify the drive */
                   const byte *buff,   /* Data to be written */
                   LBA_t       sector, /* Start sector in LBA */
                   uint        count   /* Number of sectors to write */
) {
  DRESULT res  = RES_PARERR;
  pdrv        += 0x41;
  if (!have_vdisk(pdrv)) return RES_PARERR;
  Disk_Write(sector, count, buff, pdrv);
  res = RES_OK;
  return res;
}

#endif

/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

DRESULT disk_ioctl(byte  pdrv, /* Physical drive nmuber (0..) */
                   byte  cmd,  /* Control code */
                   void *buff  /* Buffer to send/receive control data */
) {
  extern vdisk vdisk_ctl[26];
  DRESULT      res;
  int          result;

  switch (cmd) {
  case GET_SECTOR_SIZE:
    if (vdisk_ctl[pdrv].flag == 2) {
      *(u16 *)buff = 2048;
    } else {
      *(u16 *)buff = 512;
    }
    return RES_OK;
  case GET_SECTOR_COUNT: *(u32 *)buff = disk_Size(pdrv + 0x41); return RES_OK;
  case GET_BLOCK_SIZE: *(u16 *)buff = 0; return RES_OK;
  default: break;
  }

  return RES_PARERR;
}
