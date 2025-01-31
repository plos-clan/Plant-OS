#include <asio.h>
#include <libc-base.h>

typedef struct asio_regs {
  ssize_t ebx;
  ssize_t esi;
  ssize_t edi;
  ssize_t ebp;
  ssize_t esp;

#if __x86_64__
  ssize_t rbx;
  ssize_t rbp;
  ssize_t rsp;
  ssize_t r12;
  ssize_t r13;
  ssize_t r14;
  ssize_t r15;
#endif

#if __AVX__
  ssize_t ymm[16];
#endif
} *asio_regs_t;

typedef void *(*asio_func_t)(asio_t asio, void *userdata);

struct asio {
  struct asio_regs regs;
  asio_func_t      func;
  void            *stack;
  size_t           stacksize;
};

void asio_task_do_run(asio_t old, asio_t new);

void asio_save_regs(asio_regs_t regs);
void asio_restore_regs(asio_regs_t regs);

typedef void *(*asio_func_t)(asio_t, void *);

asio_t asio_task(asio_func_t func, void *userdata) {
  asio_t asio     = malloc(sizeof(struct asio));
  asio->stacksize = 4096;
  asio->stack     = malloc(asio->stacksize);
  asio->regs      = (struct asio_regs){};
  asio->regs.esp  = (size_t)asio->stack + asio->stacksize;
  return asio;
}

asio_t asio_sleep(asio_t asio, usize time_us) {
  return asio;
}

asio_t asio_run(asio_t asio) {
  return asio;
}

asio_t asio_wait(void *data) {}
asio_t asio_sync(asio_t asio) {}

// 调用调度器
void asio_schedule() {}

void asio_main() {}

#define asio_trylock()
#define asio_lock()
#define asio_unlock()
