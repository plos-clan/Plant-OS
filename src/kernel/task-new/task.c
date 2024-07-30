#include <libc.h>

rbtree_t threads;
rbtree_t processes;

typedef struct process *process_t;
typedef struct thread  *thread_t;

#define NUM_SIGNALS 32

enum {
  THREAD_IDLE,    // 线程被创建但未运行
  THREAD_RUNNING, // 线程正在运行
  THREAD_WAITING, // 线程正在等待被调度
  THREAD_STOPPED, // 线程已暂停
  THREAD_DEAD,    // 线程已结束
};

enum {
  SIGNAL_KILL  = MASK(0),  // 终止信号 强制终止程序，不会通知程序
  SIGNAL_STOP  = MASK(1),  // 暂停信号
  SIGNAL_CONT  = MASK(2),  // 继续信号
  SIGNAL_QUIT  = MASK(3),  // 退出信号 告诉程序应该退出，不是强制
  SIGNAL_ALARM = MASK(4),  // 闹钟信号
  SIGNAL_5     = MASK(5),  //
  SIGNAL_6     = MASK(6),  //
  SIGNAL_7     = MASK(7),  //
  SIGNAL_8     = MASK(8),  //
  SIGNAL_9     = MASK(9),  //
  SIGNAL_10    = MASK(10), //
  SIGNAL_11    = MASK(11), //
  SIGNAL_12    = MASK(12), //
  SIGNAL_13    = MASK(13), //
  SIGNAL_14    = MASK(14), //
  SIGNAL_15    = MASK(15), //
  SIGNAL_16    = MASK(16), //
  SIGNAL_17    = MASK(17), //
  SIGNAL_18    = MASK(18), //
  SIGNAL_19    = MASK(19), //
  SIGNAL_20    = MASK(20), //
  SIGNAL_21    = MASK(21), //
  SIGNAL_22    = MASK(22), //
  SIGNAL_23    = MASK(23), //
  SIGNAL_24    = MASK(24), //
  SIGNAL_25    = MASK(25), //
  SIGNAL_26    = MASK(26), //
  SIGNAL_27    = MASK(27), //
  SIGNAL_28    = MASK(28), //
  SIGNAL_29    = MASK(29), //
  SIGNAL_30    = MASK(30), //
  SIGNAL_31    = MASK(31), //
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

ssize_t next_tid; // 下一个线程号

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
  // TODO 开始线程 (将状态从 IDLE 调整到 RUNNING)
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
