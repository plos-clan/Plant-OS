#pragma once
#include <define.h>
#include <type.h>

struct IDEHardDiskInfomationBlock {
  char reserve1[2];
  u16  CylinesNum;
  char reserve2[2];
  u16  HeadersNum;
  u16  TrackBytes;
  u16  SectorBytes;
  u16  TrackSectors;
  char reserve3[6];
  char OEM[20];
  char reserve4[2];
  u16  BuffersBytes;
  u16  EECCheckSumLength;
  char Version[8];
  char ID[40];
};
void ide_read_buffer(u8 channel, u8 reg, u32 buffer, u32 quads);
void ide_write(u8 channel, u8 reg, u8 data);
void ide_read_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi);
void ide_write_sectors(u8 drive, u8 numsects, u32 lba, u16 es, u32 edi);
void ide_initialize(u32 BAR0, u32 BAR1, u32 BAR2, u32 BAR3, u32 BAR4);