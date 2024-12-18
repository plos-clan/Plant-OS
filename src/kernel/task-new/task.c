#include <data-structure.h>
#include <libc-base.h>

rbtree_t threads;
rbtree_t processes;

typedef struct process *process_t;
typedef struct thread  *thread_t;

#define NUM_SIGNALS 32

enum {
  THREAD_IDLE,    // 线程被创建但未运行
  THREAD_RUNNING, // 线程正在运行
  THREAD_WAITING, // 线程正在等待
  THREAD_STOPPED, // 线程已暂停
  THREAD_DEAD,    // 线程已结束
};

enum {
  SIGNAL_KILL  = MASK32(0),  // 终止信号 强制终止程序，不会通知程序
  SIGNAL_STOP  = MASK32(1),  // 暂停信号
  SIGNAL_CONT  = MASK32(2),  // 继续信号
  SIGNAL_QUIT  = MASK32(3),  // 退出信号 告诉程序应该退出，不是强制
  SIGNAL_ALARM = MASK32(4),  // 闹钟信号
  SIGNAL_5     = MASK32(5),  //
  SIGNAL_6     = MASK32(6),  //
  SIGNAL_7     = MASK32(7),  //
  SIGNAL_8     = MASK32(8),  //
  SIGNAL_9     = MASK32(9),  //
  SIGNAL_10    = MASK32(10), //
  SIGNAL_11    = MASK32(11), //
  SIGNAL_12    = MASK32(12), //
  SIGNAL_13    = MASK32(13), //
  SIGNAL_14    = MASK32(14), //
  SIGNAL_15    = MASK32(15), //
  SIGNAL_16    = MASK32(16), //
  SIGNAL_17    = MASK32(17), //
  SIGNAL_18    = MASK32(18), //
  SIGNAL_19    = MASK32(19), //
  SIGNAL_20    = MASK32(20), //
  SIGNAL_21    = MASK32(21), //
  SIGNAL_22    = MASK32(22), //
  SIGNAL_23    = MASK32(23), //
  SIGNAL_24    = MASK32(24), //
  SIGNAL_25    = MASK32(25), //
  SIGNAL_26    = MASK32(26), //
  SIGNAL_27    = MASK32(27), //
  SIGNAL_28    = MASK32(28), //
  SIGNAL_29    = MASK32(29), //
  SIGNAL_30    = MASK32(30), //
  SIGNAL_31    = MASK32(31), //
};

typedef void (*signal_t)(thread_t);

struct thread {
  process_t proc;                         // 进程
  ssize_t   tid;                          // 线程号
  ssize_t   uid;                          // 用户号
  ssize_t   euid;                         // 有效用户号 (用于指定权限)
  u8        stat;                         // 线程状态
  u32       signal_mask;                  //
  signal_t  signal_callback[NUM_SIGNALS]; //
};

struct process {
  thread_t main;     // 主线程
  list_t   threads;  //
  size_t   nthreads; //
  ssize_t  pid;
  ssize_t  uid;
  ssize_t  euid;
};

void thread_init(thread_t thread, process_t proc) {
  thread->proc        = proc;
  thread->signal_mask = 0;
}

static thread_t running_thread[32];

#define current_thread  (running_thread[cpuid_coreid])
#define current_process (running_thread[cpuid_coreid]->proc)

/**
 *\brief 切换到指定线程
 *
 *\param thread   要切换到的线程
 */
void thread_switch(thread_t thread) {}

void thread_start(thread_t thread) {
  // thread->stat = THREAD_RUNNING;
  // TODO 开始线程 (将状态从 IDLE 调整到 THREAD_RUNNING)
}

void thread_create() {

  // TODO 创建进程

  process_t proc;
  rbtree_insert(processes, proc->pid, proc);
}

// 发送一个信号
void thread_signal(thread_t thread, int id) {}

// 杀死线程，回收资源
void thread_kill(thread_t thread) {}

thread_t thread_copy(thread_t thread) {
  return null;
}

process_t process_fork(process_t process) {
  return null;
}
