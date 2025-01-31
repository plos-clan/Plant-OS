#include <kernel.h>

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE     0xf4

struct MOUSE_DEC mdec;

task_t mouse_use_task = null;

static inthandler_f inthandler2c;

static void mouse_wait(bool is_command) {
  u32 timeout = 100000;
  if (!is_command) {
    while (timeout--) { // Data
      if ((asm_in8(0x64) & 1) == 1) return;
    }
  } else {
    while (timeout--) { // Signal
      if ((asm_in8(0x64) & 2) == 0) return;
    }
  }
}

static void mouse_write(byte a_write) {
  mouse_wait(1);           // Wait to be able to send a command
  asm_out8(0x64, 0xD4);    // Tell the mouse we are sending a command
  mouse_wait(1);           // Wait for the final part
  asm_out8(0x60, a_write); // Finally write
}

// Get's response from mouse
static byte mouse_read() {
  mouse_wait(0);
  return asm_in8(0x60);
}

static void mouse_reset() {
  mouse_write(0xff);
}

void mouse_init() {
  inthandler_set(0x2c, inthandler2c);
  ps2_wait();
  asm_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  ps2_wait();
  asm_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  mouse_write(0xf3);
  mouse_write(200);
  mouse_write(0xf3);
  mouse_write(100);
  mouse_write(0xf3);
  mouse_write(80);
  mouse_write(0xf2);
  klogd("mouseId=%d\n", mouse_read());
  /* 顺利的话，键盘控制器会返回ACK(0xfa) */
}

void mouse_sleep(struct MOUSE_DEC *mdec) {
  mouse_use_task = NULL;
  mdec->sleep    = 1;
}

void mouse_ready(struct MOUSE_DEC *mdec) {
  mouse_use_task = current_task;
  mdec->sleep    = 0;
}

int mouse_decode(struct MOUSE_DEC *mdec, u8 dat) {
  if (mdec->phase == 1) {
    if (dat == 0xfa) { // ACK
      return 0;
    }
    mdec->buf[0] = dat;
    mdec->phase  = 2;
    return 0;
  }
  if (mdec->phase == 2) {
    mdec->buf[1] = dat;
    mdec->phase  = 3;
    return 0;
  }
  if (mdec->phase == 3) {
    mdec->buf[2] = dat;
    mdec->phase  = 4;
    return 0;
  }
  if (mdec->phase == 4) {
    mdec->buf[3] = dat;
    mdec->phase  = 1;
    mdec->btn    = mdec->buf[0] & 0x07;
    mdec->x      = mdec->buf[1]; // x
    mdec->y      = mdec->buf[2]; // y
    if ((mdec->buf[0] & 0x10) != 0) { mdec->x |= 0xffffff00; }
    if ((mdec->buf[0] & 0x20) != 0) { mdec->y |= 0xffffff00; }
    mdec->y = -mdec->y; //
    if (mdec->buf[3] == 0xff) {
      mdec->roll = MOUSE_ROLL_UP;
    } else if (mdec->buf[3] == 0x01) {
      mdec->roll = MOUSE_ROLL_DOWN;
    } else {
      mdec->roll = MOUSE_ROLL_NONE;
    }
    return 1;
  }
  return -1;
}

static FASTCALL void inthandler2c(i32 id, regs32 *regs) {
  byte data = asm_in8(PORT_KEYDAT);

  klogd("mouse data=%02x\n", data);
}
