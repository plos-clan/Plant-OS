#pragma once
#include <define.h>
void sendbyte(int byte);
int  getbyte();
void wait_floppy_interrupt();
void reset(void);
void floppy_init();
void recalibrate(void);
int  fdc_rw(int block, u8 *blockbuff, int read, u32 nosectors);