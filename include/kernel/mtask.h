#pragma once
#include "cpu.h"
#include "ipc.h"
#include <data-structure.h>
#include <define.h>
#include <libc-base.h>

#define SIGINT 0
#define SIGKIL 1

typedef enum ThreadState {
  THREAD_IDLE,    // 线程被创建但未运行
  THREAD_RUNNING, // 线程正在运行
  THREAD_WAITING, // 线程正在等待
  THREAD_STOPPED, // 线程已暂停
  THREAD_DEAD,    // 线程已结束
} ThreadState;

typedef void (*cb_keyboard_t)(u8 data, u32 task);

typedef struct __PACKED__ task {
  stack_frame  *esp;
  u32           pde;
  u32           user_mode;
  u32           stack_bottom; // ring0 栈底
  u32           running;      // 已经占用了多少时间片
  u32           timeout;      // 需要占用多少时间片
  int           floor;
  ThreadState   state;
  uint64_t      jiffies; // 最后一次执行的全局时间片
  i32           tid;
  i32           ptid;
  task_t        parent;
  u32           alloc_addr;
  u32          *alloc_size;
  u32           alloced;
  cir_queue8_t  press_key_fifo;  // 基本输入设备的缓冲区
  cir_queue8_t  release_keyfifo; // 基本输入设备的缓冲区
  cir_queue8_t  keyfifo;         // 基本输入设备的缓冲区
  cir_queue8_t  mousefifo;       // 基本输入设备的缓冲区
  cb_keyboard_t keyboard_press;
  cb_keyboard_t keyboard_release;
  bool          fifosleep;
  cstr          command_line;
  bool          sigint_up;
  i32           status; // 允许 0 到 255 的退出状态，绝对禁止负数
  u32           signal;
  u32           signal_disable;
  u64           cpu_time; // CPU 时间
  u32           v86_mode; // 8086模式
  u32           v86_if;   // 8086中断

  u32       rc;           // 引用计数
  list_t    waiting_list; // 正在等待该线程的线程列表
  avltree_t children;     // 子线程
  bool      is_switched;  // 是否是从其它进程切换过来的

  bool fpu_enabled;     // 是否开启了 fpu/sse/avx
  byte extra_regs[512]; // FPU 108
                        // SSE 512
                        // AVX 4096
} *task_t;

#define current_task (get_current_task())

void task_tick(); // 时间片调度
void task_next(); // 请求切换任务

task_t get_current_task();
// 将 waiting 状态的任务转换为 running 状态，并将任务设置为下一个调度任务
task_t task_run(task_t task);
// 退出当前任务
void   task_exit(i32 status) __attr(noreturn);
// 杀死任务
void   task_kill(task_t task);

/**
 *\brief 通过任务 ID 获取任务
 *
 *\param tid 任务 ID
 *\return 任务
 */
task_t task_by_id(i32 tid);

/**
 *\brief 等待任务
 *
 *\param target 目标任务
 *\return 状态码
 */
i32 task_wait(task_t target);

/**
 *\brief 将任务状态设置为等待中
 */
void task_fall_blocked();

void user_init();

/**
 *\brief 获取任务的键盘队列
 *
 *\param task 任务
 *\return 键盘队列
 */
cir_queue8_t task_get_key_queue(task_t task);

/**
 *\brief 获取任务的鼠标队列
 *
 *\param task 任务
 *\return 鼠标队列
 */
cir_queue8_t task_get_mouse_fifo(task_t task);

// 进入多任务
void into_mtask();

/**
 *\brief 创建任务
 *
 *\param entry 入口地址
 *\param ticks 时间片
 *\param floor 优先级
 *\return 任务
 */
task_t create_task(void *entry, u32 ticks, u32 floor);

/**
 *\brief 等待任务 ID
 *
 *\param tid 任务 ID
 *\return 状态码
 */
i32 waittid(i32 tid);

/**
 *\brief 设置任务的 FIFO 队列
 *
 *\param task 任务
 *\param kfifo 键盘 FIFO 队列
 *\param mfifo 鼠标 FIFO 队列
 */
void task_set_fifo(task_t task, cir_queue8_t kfifo, cir_queue8_t mfifo);

int task_fork();
int task_exec(void *entry);

#define LOCK_UNLOCKED 0
#define LOCK_LOCKED   1

void lock_init(lock_t *l);

/**
 *\brief 将任务设置为正在运行，并加入调度队列
 *
 *\param task     任务
 */
void running_tasks_push(task_t task);

/**
 *\brief 将任务从调度队列中弹出
 *
 *\return 任务
 */
task_t running_tasks_pop();

i32 os_execute(cstr filename, cstr line);
