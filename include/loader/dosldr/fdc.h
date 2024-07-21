#pragma once
#include <define.h>
#include <type.h>
void sendbyte(int byte);
int  getbyte();
void wait_floppy_interrupt();
void reset(void);
void init_floppy();
void recalibrate(void);
int  fdc_rw(int block, u8 *blockbuff, int read, u32 nosectors);