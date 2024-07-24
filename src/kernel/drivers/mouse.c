#include <kernel.h>

#define KEYCMD_SENDTO_MOUSE 0xd4
#define MOUSECMD_ENABLE     0xf4

typedef u8 byte;

mtask *mouse_use_task = NULL;

void mouse_wait(byte a_type) {
  u32 _time_out = 100000;
  if (a_type == 0) {
    while (_time_out--) { // Data
      if ((asm_in8(0x64) & 1) == 1) return;
    }
    return;
  } else {
    while (_time_out--) // Signal
    {
      if ((asm_in8(0x64) & 2) == 0) return;
    }
    return;
  }
}

void mouse_write(byte a_write) {
  // Wait to be able to send a command
  mouse_wait(1);
  // Tell the mouse we are sending a command
  asm_out8(0x64, 0xD4);
  // Wait for the final part
  mouse_wait(1);
  // Finally write
  asm_out8(0x60, a_write);
}

byte mouse_read() {
  // Get's response from mouse
  mouse_wait(0);
  return asm_in8(0x60);
}
lock_t mouse_l;
void   mouse_reset() {
  mouse_write(0xff);
}
void enable_mouse(struct MOUSE_DEC *mdec) {
  lock_init(&mouse_l);
  /* 激活鼠标 */
  wait_KBC_sendready();
  asm_out8(PORT_KEYCMD, KEYCMD_SENDTO_MOUSE);
  wait_KBC_sendready();
  asm_out8(PORT_KEYDAT, MOUSECMD_ENABLE);
  mdec->phase = 1;
  mouse_write(0xf3);
  mouse_write(200);
  mouse_write(0xf3);
  mouse_write(100);
  mouse_write(0xf3);
  mouse_write(80);
  mouse_write(0xf2);
  logd("mouseId=%d\n", mouse_read());
  /* 顺利的话，键盘控制器会返回ACK(0xfa) */
}

void mouse_sleep(struct MOUSE_DEC *mdec) {
  mouse_use_task = NULL;
  mdec->sleep    = 1;
}

void mouse_ready(struct MOUSE_DEC *mdec) {
  mouse_use_task = current_task();
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
  } else if (mdec->phase == 2) {
    mdec->buf[1] = dat;
    mdec->phase  = 3;
    return 0;
  } else if (mdec->phase == 3) {
    mdec->buf[2] = dat;
    mdec->phase  = 4;
    return 0;
  } else if (mdec->phase == 4) {
    // printk("已经收集了四个字节\n");
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

// int a = 1;
u32 m_cr3 = 0;
u32 m_eip = 0;
u32 times = 0;

void inthandler2c(int *esp) {
  // logd("2c\n");
  u8 data;
  asm_out8(PIC1_OCW2, 0x64);
  asm_out8(PIC0_OCW2, 0x62);
  data = asm_in8(PORT_KEYDAT);
  times++;
  if (times == 4) {
    times = 0;
    if (mouse_use_task != NULL || !mouse_use_task->fifosleep || mouse_use_task->state == 1) {
      //   logd("put %08x\n",task_get_mouse_fifo(mouse_use_task));
      circular_queue_put(task_get_mouse_fifo(mouse_use_task), data);

      if (current_task() != mouse_use_task) {
        //   logd("SET 1\n");
        mouse_use_task->timeout = 5;
        mouse_use_task->ready   = 1;
        mouse_use_task->urgent  = 1;
        mouse_use_task->running = 0;
        mtask_run_now(mouse_use_task);
        task_next();
      } else {
        mouse_use_task->running = 0;
      }
    }
  } else {
    if (mouse_use_task != NULL || !mouse_use_task->fifosleep || mouse_use_task->state == 1) {
      //   logd("put %08x\n",task_get_mouse_fifo(mouse_use_task));
      circular_queue_put(task_get_mouse_fifo(mouse_use_task), data);
    }
  }
}