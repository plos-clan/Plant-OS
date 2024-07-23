#include <libc.h>

rbtree_t threads;
rbtree_t processes;

typedef struct process *process_t;
typedef struct thread  *thread_t;

#define NUM_SIGNALS 32

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
