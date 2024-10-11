/*
 * Copyright by GazOS
 * min0911 & zhouzhihao 对其进行略微修改并移植
 */
#include <loader.h>
volatile int floppy_int_count = 0;
void         floppy_int();
typedef struct DrvGeom {
  u8 heads;
  u8 tracks;
  u8 spt; /* 每轨扇区数 */
} DrvGeom;

/* 驱动器结构 */
#define DG144_HEADS   2    /* 每个磁道中的磁头数 (1.44M) */
#define DG144_TRACKS  80   /* 每个驱动器的磁道数 (1.44M) */
#define DG144_SPT     18   /* 每个磁头中的的扇区数 (1.44M) */
#define DG144_GAP3FMT 0x54 /* GAP3格式化 (1.44M) */
#define DG144_GAP3RW  0x1b /* GAP3（读/写） (1.44M) */

#define DG168_HEADS   2    /* 每个磁道中的磁头数 (1.68M) */
#define DG168_TRACKS  80   /* 每个驱动器的磁道数 (1.68M) */
#define DG168_SPT     21   /* 每个磁头中的的扇区数 (1.68M) */
#define DG168_GAP3FMT 0x0c /* GAP3格式化 (1.68M) */
#define DG168_GAP3RW  0x1c /* GAP3（读/写） (1.68M) */

/* i/o端口定义 */
#define FDC_DOR  (0x3f2) /* 数字输出寄存器 */
#define FDC_MSR  (0x3f4) /* 主要状态寄存器（输入） */
#define FDC_DRS  (0x3f4) /* DRS寄存器 */
#define FDC_DATA (0x3f5) /* 数据寄存器 */
#define FDC_DIR  (0x3f7) /* 数字输入寄存器（输入） */
#define FDC_CCR  (0x3f7) /* CCR寄存器 */

/* 软盘命令 */
#define CMD_SPECIFY (0x03) /* 指定驱动器计时 */
#define CMD_WRITE   (0xc5) /* 写（写入数据的最小单位是扇区） */
#define CMD_READ    (0xe6) /* 读（读取扇区的最小单位是扇区） */
#define CMD_RECAL   (0x07) /* 重新校准软盘 */
#define CMD_SENSEI  (0x08) /* 中断状态 */
#define CMD_FORMAT  (0x4d) /* 格式化磁道 */
#define CMD_SEEK    (0x0f) /* 寻找磁道 */
#define CMD_VERSION (0x10) /* 获取软盘驱动器的版本 */

/**globals*/
static volatile int done      = 0;
static int          dchange   = 0;
int                 motor     = 0;
int                 mtick     = 0;
static volatile int tmout     = 0;
static u8           status[7] = {0};
static u8           statsz    = 0;
static u8           sr0       = 0;
static u8           fdc_track = 0xff;
static DrvGeom      geometry  = {DG144_HEADS, DG144_TRACKS, DG144_SPT};
u32                 tbaddr    = 0x80000L; /* 位于1M以下的轨道缓冲器的物理地址 */
static void         Read(int drive, u8 *buffer, u32 number, u32 lba) {
  fdc_rw(lba, buffer, 1, number);
}
static void Write(int drive, u8 *buffer, u32 number, u32 lba) {
  fdc_rw(lba, buffer, 0, number);
}
void floppy_init() {
#ifndef __NO_FLOPPY__
  sendbyte(CMD_VERSION); //发送命令（获取软盘版本），如果收到回应，说明软盘正在工作
  if (getbyte() == -1) {
    klogf("floppy: no floppy drive found");
    klogf("No fount FDC");
    return;
  }
  //设置软盘驱动器的中断服务程序
  struct GATE_DESCRIPTOR *idt = (struct GATE_DESCRIPTOR *)ADR_IDT;
  set_gatedesc(idt + 0x26, (int)floppy_int, 4 * 8, AR_INTGATE32);
  ClearMaskIrq(0x6); //清除IRQ6的中断
  klogf("FLOPPY DISK:RESETING\n");
  reset(); //重置软盘驱动器
  klogf("FLOPPY DISK:reset over!\n");
  sendbyte(CMD_VERSION);              //获取软盘版本
  klogf("FDC_VER:0x%x\n", getbyte()); //并且输出到屏幕上
#endif
  vdisk vd;
  strcpy(vd.DriveName, "floppy");
  vd.Read  = Read;
  vd.Write = Write;
  vd.size  = 1474560;
  vd.flag  = 1;
  register_vdisk(vd);
}

void flint(int *esp) {
  /**
   * 软盘中断服务程序（C语言），这个中断的入口在nasmfunc.asm中
   * */
  //print("flint.?\n");
  floppy_int_count = 1; // 设置中断计数器为1，代表中断已经发生（或者是系统已经收到了中断）
  asm_out8(0x20, 0x20); // 发送EOI信号，告诉PIC，我们已经处理完了这个中断
}
void reset() {
  /* 停止软盘电机并禁用IRQ和DMA传输 */
  asm_out8(FDC_DOR, 0);

  //for(;;);
  //初始化电机计数器
  mtick = 0;
  motor = 0;

  /* 数据传输速度 (500K/s) */
  asm_out8(FDC_DRS, 0);
  /* 重新启动软盘中断（让软盘发送iRQ6），这将会调用上面的flint函数 */
  asm_out8(FDC_DOR, 0x0c);

  /* 重置软盘驱动器将会引发一个中断了，我们需要进行处理 */
  //for(;;);
  wait_floppy_interrupt(); //等待软盘驱动器的中断发生

  /* 指定软盘驱动器定时（不使用在实模式时BIOS设定的操作） */
  sendbyte(CMD_SPECIFY);
  sendbyte(0xdf); /* SRT = 3ms, HUT = 240ms */
  sendbyte(0x02); /* HLT = 16ms, ND = 0 */

  /* 清除“磁盘更改”状态 */
  recalibrate();

  dchange = 0; //清除“磁盘更改”状态（将dchange设置为false，让别的函数知道磁盘更改状态已经被清楚了）
}
void motoron() {
  if (!motor) {
    mtick = -1; /* 停止电机计时 */
    asm_out8(FDC_DOR, 0x1c);
    for (int i = 0; i < 80000; i++)
      ;
    motor = 1; //设置电机状态为true
  }
}

/* 关闭电机 */
void motoroff() {
  if (motor) { mtick = 13500; /* 重新初始化电机计时器 */ }
}

/* 重新校准驱动器 */
void recalibrate() {
  /* 先启用电机 */
  motoron();

  /* 然后重新校准电机 */
  sendbyte(CMD_RECAL);
  sendbyte(0);

  /* 等待软盘中断（也就是电机校准成功） */
  wait_floppy_interrupt();
  /* 关闭电机 */
  motoroff();
}
int seek(int track) {
  if (fdc_track == track) /* 目前的磁道和需要seek的磁道一样吗 */
  {
    // 一样的话就不用seek了
    return 1;
  }

  /* 向软盘控制器发送SEEK命令 */
  sendbyte(CMD_SEEK);
  sendbyte(0);
  sendbyte(track); /* 要seek到的磁道号 */

  /* 发送完之后，软盘理应会送来一个中断 */
  wait_floppy_interrupt(); // 所以我们需要等待软盘中断

  /* 然后我们等待软盘seek（大约15ms） */
  // sleep(1); // 注意：这里单位是hz，1hz=10ms，所以sleep(1)就是sleep(10)
  // 用for循环 等待15微秒
  for (int i = 0; i < 500; i++)
    ;
  /* 检查一下成功了没有 */
  if ((sr0 != 0x20) || (fdc_track != track))
    return 0; // 没成功
  else
    return 1; // 成功了
}
void sendbyte(int byte) // 向软盘控制器发送一个字节
{
  volatile int msr; // 注意：这里是volatile，这样可以保证msr的值不会被优化掉
  int          tmo; // 超时计数器

  for (tmo = 0; tmo < 128; tmo++) // 这里我们只给128次尝试的机会
  {
    msr = asm_in8(FDC_MSR);
    if ((msr & 0xc0) == 0x80) // 如果软盘驱动器的状态寄存器的低6位是0x80，说明软盘能够接受新的数据
    {
      // 哈，程序如果执行到这里，可不就说明软盘驱动器可以接受新的数据了吗？
      // 那就发送呗
      asm_out8(FDC_DATA, byte);
      return;
    }
    asm_in8(0x80); /* 等待 */
  }
}
int getbyte() {
  int msr; // 软盘驱动器状态寄存器
  int tmo; // 软盘驱动器状态寄存器的超时计数器

  for (tmo = 0; tmo < 128; tmo++) // 这里我们只给128次尝试的机会
  {
    msr = asm_in8(FDC_MSR);
    if ((msr & 0xd0) ==
        0xd0) // 如果软盘控制器的状态寄存器的低五位是0xd0，说明我们能够从软盘DATA寄存器中读取
    {
      // 能读取了？那就读取吧，读完再返回回去
      return asm_in8(FDC_DATA);
    }
    asm_in8(0x80); /* 延时 */
  }
  return -1; /* 没读取到 */
}
void wait_floppy_interrupt() {
  waitif(!floppy_int_count);
  statsz = 0; // 清空状态
  while (
      (statsz < 7) &&
      (asm_in8(FDC_MSR) &
       (1
        << 4))) //  状态寄存器的低四位是1（TRUE，所以这里不用写==），说明软盘驱动器没发送完所有的数据，当我们获取完所有的数据（状态变量=7），就可以跳出循环了
  {
    status[statsz++] = getbyte(); // 获取数据
  }

  /* 获取中断状态 */
  sendbyte(CMD_SENSEI);
  sr0       = getbyte();
  fdc_track = getbyte();

  floppy_int_count = 0;
  return;
}
void block2hts(int block, int *track, int *head, int *sector) {
  *track  = (block / 18) / 2;
  *head   = (block / 18) % 2;
  *sector = block % 18 + 1;
}
void hts2block(int track, int head, int sector, int *block) {
  *block = track * 18 * 2 + head * 18 + sector;
}
int fdc_rw(int block, u8 *blockbuff, int read, u32 nosectors) {
  int head, track, sector, tries, copycount = 0;
  u8 *p_tbaddr = (void *)0x80000; // 512byte
      // 缓冲区（大家可以放心，我们在page.c已经把这里设置为占用了）
  u8 *p_blockbuff = blockbuff; // r/w的数据缓冲区

  /* 获取block对应的ths */
  block2hts(block, &track, &head, &sector);

  motoron();

  if (!read && blockbuff) {
    /* 从数据缓冲区复制数据到轨道缓冲区 */
    for (copycount = 0; copycount < (nosectors * 512); copycount++) {
      *p_tbaddr = *p_blockbuff;
      p_blockbuff++;
      p_tbaddr++;
    }
  }

  for (tries = 0; tries < 3; tries++) {
    /* 检查 */
    if (asm_in8(FDC_DIR) & 0x80) {
      dchange = 1;
      seek(1); /* 清除磁盘更改 */
      recalibrate();
      motoroff();

      return fdc_rw(block, blockbuff, read, nosectors);
    }
    /* seek到track的位置*/
    if (!seek(track)) {
      motoroff();
      return 0;
    }

    /* 传输速度（500K/s） */
    asm_out8(FDC_CCR, 0);

    /* 发送命令 */
    if (read) {
      dma8_recv(2, (void *)tbaddr, nosectors * 512);
      sendbyte(CMD_READ);
    } else {
      dma8_send(2, (void *)tbaddr, nosectors * 512);
      sendbyte(CMD_WRITE);
    }

    sendbyte(head << 2);
    sendbyte(track);
    sendbyte(head);
    sendbyte(sector);
    sendbyte(2); /* 每个扇区是512字节 */
    sendbyte(geometry.spt);

    if (geometry.spt == DG144_SPT) {
      sendbyte(DG144_GAP3RW);
    } else {
      sendbyte(DG168_GAP3RW);
    }
    sendbyte(0xff);

    /* 等待中断...... */
    /* 读写数据不需要中断状态 */
    wait_floppy_interrupt();

    if ((status[0] & 0xc0) == 0) break; /* worked! outta here! */

    recalibrate(); /* az，报错了，再试一遍 */
  }

  /* 关闭电动机 */
  motoroff();

  if (read && blockbuff) {
    /* 复制数据 */
    p_blockbuff = blockbuff;
    p_tbaddr    = (void *)0x80000;
    for (copycount = 0; copycount < (nosectors * 512); copycount++) {
      *p_blockbuff = *p_tbaddr;
      p_blockbuff++;
      p_tbaddr++;
    }
  }

  return (tries != 3);
}
int fdc_rw_ths(int track, int head, int sector, u8 *blockbuff, int read, u32 nosectors) {
  // 跟上面的大同小异
  int tries, copycount = 0;
  u8 *p_tbaddr    = (void *)0x80000;
  u8 *p_blockbuff = blockbuff;

  motoron();

  if (!read && blockbuff) {
    for (copycount = 0; copycount < (nosectors * 512); copycount++) {
      *p_tbaddr = *p_blockbuff;
      p_blockbuff++;
      p_tbaddr++;
    }
  }

  for (tries = 0; tries < 3; tries++) {
    if (asm_in8(FDC_DIR) & 0x80) {
      dchange = 1;
      seek(1);
      recalibrate();
      motoroff();

      return fdc_rw_ths(track, head, sector, blockbuff, read, nosectors);
    }
    if (!seek(track)) {
      motoroff();
      return 0;
    }

    asm_out8(FDC_CCR, 0);

    if (read) {
      dma8_recv(2, (void *)tbaddr, nosectors * 512);
      sendbyte(CMD_READ);
    } else {
      dma8_send(2, (void *)tbaddr, nosectors * 512);
      sendbyte(CMD_WRITE);
    }

    sendbyte(head << 2);
    sendbyte(track);
    sendbyte(head);
    sendbyte(sector);
    sendbyte(2);
    sendbyte(geometry.spt);

    if (geometry.spt == DG144_SPT) {
      sendbyte(DG144_GAP3RW);
    } else {
      sendbyte(DG168_GAP3RW);
    }
    sendbyte(0xff);

    wait_floppy_interrupt();

    if ((status[0] & 0xc0) == 0) break;

    recalibrate();
  }

  motoroff();

  if (read && blockbuff) {
    p_blockbuff = blockbuff;
    p_tbaddr    = (void *)0x80000;
    for (copycount = 0; copycount < (nosectors * 512); copycount++) {
      *p_blockbuff = *p_tbaddr;
      p_blockbuff++;
      p_tbaddr++;
    }
  }

  return (tries != 3);
}
int read_block(int block, u8 *blockbuff, u32 nosectors) {
  int track = 0, sector = 0, head = 0, track2 = 0, result = 0, loop = 0;
  block2hts(block, &track, &head, &sector);
  block2hts(block + nosectors, &track2, &head, &sector);

  if (track != track2) {
    for (loop = 0; loop < nosectors; loop++)
      result = fdc_rw(block + loop, blockbuff + (loop * 512), 1, 1);
    return result;
  }
  return fdc_rw(block, blockbuff, 1, nosectors);
}

/* 写一个扇区 */
int write_block(int block, u8 *blockbuff, u32 nosectors) {
  return fdc_rw(block, blockbuff, 0, nosectors);
}
int write_floppy_for_ths(int track, int head, int sec, u8 *blockbuff, u32 nosec) {
  return fdc_rw_ths(track, head, sec, blockbuff, 0, nosec);
}
