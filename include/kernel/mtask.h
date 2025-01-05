#pragma once
#include "00-include.h"
#include "cpu.h"
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

typedef struct task {
  union {
    byte _kernel_part[PAGE_SIZE];
#define _NAME
#include "task/kernel-part"
  };
  union {
    byte _user_part[PAGE_SIZE];
#define _NAME
#include "task/user-part"
  };
  byte kernel_extra_regs[4096]; // FPU 108
                                // SSE 512
                                // AVX 4096
  byte extra_regs[4096];        // FPU 108
                                // SSE 512
                                // AVX 4096
} *task_t;

#define current_task (get_current_task())

void fpu_copy_ctx(task_t dst, task_t src);

task_t task_ref(task_t t);
void   task_unref(task_t t);

void task_tick(); // 时间片调度
void task_next(); // 请求切换任务

task_t get_current_task();
// 将 waiting 状态的任务转换为 running 状态，并将任务设置为下一个调度任务
task_t task_run(task_t task);
// 退出当前任务
void   task_exit(i32 status) __attr(noreturn);
// 杀死任务
void   task_kill(task_t task);
// 异常退出
void   task_abort() __attr(noreturn);

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
