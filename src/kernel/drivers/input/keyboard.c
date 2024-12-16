#include <kernel.h>

#define KEYSTA_SEND_NOTREADY 0x02
#define KEYCMD_WRITE_MODE    0x60
#define KBC_MODE             0x47

static int input_char_inSM();

static int caps_lock, shift, e0_flag = 0, ctrl = 0;

static char keytable[0x54] = { // 按下Shift
    0,   0x01, '!', '@', '#', '$', '%',  '^', '&', '*', '(', ')', '_', '+', '\b', '\t', 'Q',
    'W', 'E',  'R', 'T', 'Y', 'U', 'I',  'O', 'P', '{', '}', 10,  0,   'A', 'S',  'D',  'F',
    'G', 'H',  'J', 'K', 'L', ':', '\"', '~', 0,   '|', 'Z', 'X', 'C', 'V', 'B',  'N',  'M',
    '<', '>',  '?', 0,   '*', 0,   ' ',  0,   0,   0,   0,   0,   0,   0,   0,    0,    0,
    0,   0,    0,   '7', 'D', '8', '-',  '4', '5', '6', '+', '1', '2', '3', '0',  '.'};

static char keytable1[0x54] = { // 未按下Shift
    0,   0x01, '1', '2', '3', '4', '5',  '6', '7', '8',  '9', '0', '-', '=', '\b', '\t', 'q',
    'w', 'e',  'r', 't', 'y', 'u', 'i',  'o', 'p', '[',  ']', 10,  0,   'a', 's',  'd',  'f',
    'g', 'h',  'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z', 'x', 'c', 'v', 'b',  'n',  'm',
    ',', '.',  '/', 0,   '*', 0,   ' ',  0,   0,   0,    0,   0,   0,   0,   0,    0,    0,
    0,   0,    0,   '7', '8', '9', '-',  '4', '5', '6',  '+', '1', '2', '3', '0',  '.'};

// 等待键盘控制电路准备完毕
void wait_KBC_sendready() {
  while (true) {
    if ((asm_in8(PORT_KEYSTA) & KEYSTA_SEND_NOTREADY) == 0) { break; }
  }
}

static inthandler_f inthandler21;

// 初始化键盘控制电路
void init_keyboard() {
  inthandler_set(0x21, inthandler21);
  wait_KBC_sendready();
  asm_out8(PORT_KEYCMD, KEYCMD_WRITE_MODE);
  wait_KBC_sendready();
  asm_out8(PORT_KEYDAT, KBC_MODE);
}

int getch() {
  u8 ch;
  ch = input_char_inSM(); // 扫描码
  if (ch == 0xe0) {       // keytable之外的键（↑,↓,←,→）
    ch = input_char_inSM();
    if (ch == 0x48) { // ↑
      return -1;
    } else if (ch == 0x50) { // ↓
      return -2;
    } else if (ch == 0x4b) { // ←
      return -3;
    } else if (ch == 0x4d) { // →
      return -4;
    }
  }
  // 返回扫描码（keytable之内）对应的ASCII码
  if (keytable[ch] == 0x00) {
    return 0;
  } else if (shift == 0 && caps_lock == 0) {
    return keytable1[ch];
  } else if (shift == 1 || caps_lock == 1) {
    return keytable[ch];
  } else if (shift == 1 && caps_lock == 1) {
    return keytable1[ch];
  }
  return -1;
}

extern struct tty *tty_default;

int tty_fifo_status() {
  return tty_default->fifo_status(tty_default);
}

int tty_fifo_get() {
  return tty_default->fifo_get(tty_default);
}

static int input_char_inSM() {
  int    i;
  task_t task = current_task;
  while (1) {
    if ((tty_fifo_status() != 0)) {
      // 返回扫描码
      i = tty_fifo_get(); // 从FIFO缓冲区中取出扫描码
      if (i != -1) break;
    }
  }
  return i;
}

// 扫描码转化ASCII码
// 逻辑与getch函数大同小异
static int sc2a(int sc) {
  int ch = sc;
  if (ch > 0x80) {
    ch -= 0x80;
    if (ch == 0x48) return -1;
    if (ch == 0x50) return -2;
    if (ch == 0x4b) return -3;
    if (ch == 0x4d) return -4;
  }
  if (keytable[ch] == 0x00) return 0;
  if (shift == 0 && caps_lock == 0) return keytable1[ch];
  if (shift == 1 || caps_lock == 1) return keytable[ch];
  if (shift == 1 && caps_lock == 1) return keytable1[ch];
  return -1;
}

int    disable_flag      = 0;
task_t keyboard_use_task = NULL;

static void inthandler21(i32 id, regs32 *regs) {
  // 键盘中断处理函数
  u8 data, s[4];
  asm_out8(PIC0_OCW2, 0x61);
  data = asm_in8(PORT_KEYDAT); // 从键盘IO口读取扫描码
  // klogd("%d", data);
  //return;
  //  特殊键处理
  if (data == 0xe0) {
    e0_flag = 1;
    return;
  }
  if (data == 0x2a || data == 0x36) { // Shift按下
    shift = 1;
  }
  if (data == 0x1d) { // Ctrl按下
    ctrl = 1;
  }
  if (data == 0x3a) { // Caps Lock按下
    caps_lock = caps_lock ^ 1;
  }
  if (data == 0xaa || data == 0xb6) { // Shift松开
    shift = 0;
  }
  if (data == 0x9d) { // Ctrl按下
    ctrl = 0;
  }
  // 快捷键处理
  if (data == 0x2e && ctrl) {
    for (int i = 0; i < 255; i++) {
      if (!task_by_id(i)) { continue; }
      if (task_by_id(i)->sigint_up) { task_by_id(i)->signal |= MASK32(SIGINT); }
    }
    // return;
  }
  // } else if (data >= 0x3b && data <= 0x47 && shift) {
  //   // 按下F1 ~ F12 & Shift
  //   // if (running_mode == POWERINTDOS) {
  //   //   SwitchShell_TextMode(data - 0x3b + 1);  // 换到1~12号控制台
  //   //   io_sti();
  //   //   return;
  //   // } else if (running_mode == HIGHTEXTMODE) {
  //   //   SwitchShell_HighTextMode(data - 0x3b + 1);
  //   //   io_sti();
  //   //   return;
  //   // }
  // }
  // 普通键处理
  if (data >= 0x80) {
    // printk("press\n");
    if (disable_flag && keyboard_use_task) {
      if (keyboard_use_task->keyboard_release != NULL) {
        // TASK结构体中有对按下键特殊处理的
        if (e0_flag) { keyboard_use_task->keyboard_release(0xe0, keyboard_use_task->tid); }
        keyboard_use_task->keyboard_release(data, keyboard_use_task->tid); // 处理按下键
      }
      if (current_task != keyboard_use_task) {
        keyboard_use_task->timeout = 5;
        keyboard_use_task->running = 0;
        task_run(keyboard_use_task);
        task_next();
      } else {
        keyboard_use_task->running = 0;
      }
    } else
      for (int i = 0; i < 255; i++) {
        if (!task_by_id(i)) { continue; }
        if (task_by_id(i)->keyboard_release != NULL) {
          // TASK结构体中有对松开键特殊处理的
          if (e0_flag) { task_by_id(i)->keyboard_release(0xe0, i); }
          task_by_id(i)->keyboard_release(data, i); // 处理松开键

          if (disable_flag) {}
        }
      }
    if (e0_flag == 1) e0_flag = 0;
    return;
  }
  if (disable_flag && keyboard_use_task) {
    if (keyboard_use_task->keyboard_press != NULL) {
      // TASK结构体中有对按下键特殊处理的
      if (e0_flag) { keyboard_use_task->keyboard_press(0xe0, keyboard_use_task->tid); }
      keyboard_use_task->keyboard_press(data,
                                        keyboard_use_task->tid); // 处理按下键
    }
    if (current_task != keyboard_use_task) {
      //   klogd("SET 1\n");
      keyboard_use_task->timeout = 5;
      keyboard_use_task->running = 0;
      task_run(keyboard_use_task);
      task_next();
    } else {
      keyboard_use_task->running = 0;
    }
  } else
    for (int i = 0; i < 255; i++) {
      // printk("up\n");
      if (!task_by_id(i)) { continue; }
      if (task_by_id(i)->keyboard_press != NULL) {
        // TASK结构体中有对按下键特殊处理的
        if (e0_flag) { task_by_id(i)->keyboard_press(0xe0, i); }
        task_by_id(i)->keyboard_press(data, i); // 处理按下键
      }
    }
  if (disable_flag == 0)
    for (int i = 0; i < 255; i++) {
      if (!task_by_id(i)) { continue; }
      // 按下键通常处理
      task_t task = task_by_id(i); // 每个进程都处理一遍
      if (task->state != THREAD_RUNNING || task->fifosleep) {
        // if (task->state == THREAD_WAITING && task->waittid < 0) { goto THROUGH; }
        // 如果进程正在休眠或被锁了
        continue;
      }
      // 一般进程
    THROUGH:

      if (task_get_key_queue(task)) {
        if (e0_flag) { cir_queue8_put(task_get_key_queue(task), 0xe0); }
        cir_queue8_put(task_get_key_queue(task), data);
      }
    }
  if (e0_flag == 1) e0_flag = 0;
}
