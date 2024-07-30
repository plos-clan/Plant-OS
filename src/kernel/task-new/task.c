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
  SIGNAL_KILL,  // 终止信号 强制终止程序，不会通知程序
  SIGNAL_STOP,  // 暂停信号
  SIGNAL_CONT,  // 继续信号
  SIGNAL_QUIT,  // 退出信号 告诉程序应该退出，不是强制
  SIGNAL_ALARM, // 闹钟信号
};

struct thread {
  process_t proc;
  ssize_t   tid;
  ssize_t   uid;
  ssize_t   euid;
  BITMAP(signal_mask, NUM_SIGNALS);
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
  thread->proc = proc;
  bitmap_clearall(thread->signal_mask, NUM_SIGNALS);
}

void thread_start() {}

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
