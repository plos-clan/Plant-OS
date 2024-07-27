#include <kernel.h>
#include <cpu.h>

void mtask_stop();
void mtask_start();

extern char mtask_stop_flag;

bool cas(int *ptr, int old, int New) {
  int old_value = *ptr;
  if (old_value == old) {
    *ptr = New;
    return true;
  }
  return false;
}

void lock_init(lock_t *l) {
  l->owner  = NULL;
  l->value  = LOCK_UNLOCKED;
  l->waiter = NULL;
}

bool interrupt_disable() {
  size_t flags;
  asm volatile("pushfl\n"
               "cli\n"
               "popl %%eax\n"
               : "=a"(flags)
               :
               : "memory");
  return (flags >> 9) & 1;
}

// 获得 IF 位
bool get_interrupt_state() {
  size_t flags;
  asm volatile("pushfl\n\t"
               "popl %%eax\n\t"
               : "=a"(flags)
               :
               : "memory");
  // 将 eax 右移 9 位，得到 IF 位
  return (flags >> 9) & 1;
}

// 设置 IF 位
void set_interrupt_state(bool state) {
  if (state) {
    asm_sti;
  } else {
    asm_cli;
  }
}
void lock(lock_t *key) {
  int state = interrupt_disable();
  if (key->value != LOCK_UNLOCKED) {
    key->waiter = current_task();
    if (key->value != LOCK_UNLOCKED && key->waiter) {

      mtask_run_now(key->owner);
      if (current_task()->ready == 1) { current_task()->ready = 0; }
      task_fall_blocked(WAITING);
    }
  }
  interrupt_disable();
  key->waiter = NULL;
  key->value  = LOCK_LOCKED;
  key->owner  = current_task();
  set_interrupt_state(state);
}
void unlock(lock_t *key) {
  int state  = interrupt_disable();
  key->value = LOCK_UNLOCKED;
  if (key->waiter) {
    mtask_run_now(key->waiter);
    task_run(key->waiter);
    key->waiter = NULL;
    task_next();
  }
  set_interrupt_state(state);
}