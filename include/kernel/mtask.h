#pragma once
#include <data-structure.h>
#include <define.h>
#include <kernel/cpu.h>
#include <kernel/ipc.h>
#include <kernel/mem.h>
#include <libc-base.h>

#define SIGINT 0
#define SIGKIL 1

enum STATE {
  EMPTY,
  RUNNING,
  WAITING,
  SLEEPING,
  WILL_EMPTY,
  READY,
  DIED
};

typedef void (*cb_keyboard_t)(u8 data, u32 task);

struct __PACKED__ mtask {
  stack_frame  *esp;
  u32           pde;
  u32           user_mode;
  u32           top;
  u32           running; // 已经占用了多少时间片
  u32           timeout; // 需要占用多少时间片
  int           floor;
  enum STATE    state;   // 此项为1（RUNNING） 即正常调度，为 2（WAITING） 3
                         // （SLEEPING）的时候不执行 ，0 EMPTY 空闲格子
  uint64_t      jiffies; // 最后一次执行的全局时间片
  // struct vfs_t *nfs;
  i32           tid, ptid;
  u32           alloc_addr;
  u32          *alloc_size;
  u32           alloced;
  struct tty   *TTY;
  int           DisableExpFlag;
  u32           CatchEIP;
  char          flagOfexp;
  fpu_regs_t    fpu;
  int           fpu_flag;
  cir_queue8_t  press_key_fifo, release_keyfifo;
  cir_queue8_t  keyfifo, mousefifo; // 基本输入设备的缓冲区
  char          urgent;
  cb_keyboard_t keyboard_press;
  cb_keyboard_t keyboard_release;
  char          fifosleep;
  int           mx, my;
  char         *line;
  struct TIMER *timer;
  IPC_Header    ipc_header;
  u32           waittid;
  int           ready; // 如果为waiting 则无视wating
  int           sigint_up;
  u8            train; // 轮询
  u32           status;
  u32           signal;
  u32           handler[30];
  u32           ret_to_app;
  u32           times;
  u32           signal_disable;
  list_t        subtasks; // 子任务
  u64           cpu_time; // CPU 时间
  u32           v86_mode; // 8086模式
  u32           v86_if;   // 8086中断
};

mtask *current_task();
void   task_switch(mtask *next);  // 切换任务
void   task_start(mtask *next);   // 开始任务
void   mtask_run_now(mtask *obj); // 立即执行任务
void   task_run(mtask *task);
void   task_next();
void   task_exit(u32 status);
mtask *get_task(u32 tid);

#define get_tid(n)     ((n)->tid)
#define offsetof(s, m) (size_t)&(((s *)0)->m)

void task_fall_blocked(enum STATE state);
void idle_loop();
void init();

cir_queue8_t task_get_key_queue(mtask *task);
cir_queue8_t task_get_mouse_fifo(mtask *task);
void         into_mtask();
mtask       *create_task(void *func, u32 ticks, u32 floor);
void         task_exit(u32 status);
int          waittid(u32 tid);
void         task_set_fifo(mtask *task, cir_queue8_t kfifo, cir_queue8_t mfifo);
int          task_fork();
