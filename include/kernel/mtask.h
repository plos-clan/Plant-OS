#pragma once
#include "cpu.h"
#include "ipc.h"
#include <data-structure.h>
#include <define.h>
#include <libc-base.h>

#define SIGINT 0
#define SIGKIL 1

enum STATE {
  EMPTY,
  RUNNING,
  WAITING,
  SLEEPING,
  READY,
  DIED
};

typedef void (*cb_keyboard_t)(u8 data, u32 task);

typedef struct __PACKED__ task {
  stack_frame  *esp;
  u32           pde;
  u32           user_mode;
  u32           stack_bottom; // ring0 栈底
  u32           running;      // 已经占用了多少时间片
  u32           timeout;      // 需要占用多少时间片
  int           floor;
  enum STATE    state;   // 此项为1（RUNNING） 即正常调度，为 2（WAITING） 3
                         // （SLEEPING）的时候不执行 ，0 EMPTY 空闲格子
  uint64_t      jiffies; // 最后一次执行的全局时间片
  i32           tid;
  i32           ptid;
  u32           alloc_addr;
  u32          *alloc_size;
  u32           alloced;
  struct tty   *TTY;
  fpu_regs_t    fpu;
  bool          fpu_enabled;
  cir_queue8_t  press_key_fifo;  // 基本输入设备的缓冲区
  cir_queue8_t  release_keyfifo; // 基本输入设备的缓冲区
  cir_queue8_t  keyfifo;         // 基本输入设备的缓冲区
  cir_queue8_t  mousefifo;       // 基本输入设备的缓冲区
  bool          urgent;
  cb_keyboard_t keyboard_press;
  cb_keyboard_t keyboard_release;
  bool          fifosleep;
  const char   *line;
  struct TIMER *timer;
  i32           waittid;
  bool          sigint_up;
  u32           status;
  u32           signal;
  u32           times;
  u32           signal_disable;
  u64           cpu_time; // CPU 时间
  u32           v86_mode; // 8086模式
  u32           v86_if;   // 8086中断

  u32 rc; // 引用计数
} *task_t;

#define current_task (get_current_task())

task_t get_current_task();
void   task_switch(task_t next);  // 切换任务
void   task_start(task_t next);   // 开始任务
void   mtask_run_now(task_t obj); // 立即执行任务
void   task_run(task_t task);
void   task_next();
void   task_exit(u32 status);
task_t get_task(u32 tid);

void task_fall_blocked(enum STATE state);
void user_init();

cir_queue8_t task_get_key_queue(task_t task);
cir_queue8_t task_get_mouse_fifo(task_t task);
void         into_mtask();
task_t       create_task(void *func, u32 ticks, u32 floor);
int          waittid(u32 tid);
void         task_set_fifo(task_t task, cir_queue8_t kfifo, cir_queue8_t mfifo);
int          task_fork();

#define LOCK_UNLOCKED 0
#define LOCK_LOCKED   1

void lock_init(lock_t *l);

void   running_tasks_push(task_t task);
task_t running_tasks_pop();
