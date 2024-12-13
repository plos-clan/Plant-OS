// This code is released under the MIT License

#include <kernel.h>

void lock_init(lock_t *l) {
  l->owner  = NULL;
  l->value  = LOCK_UNLOCKED;
  l->waiter = NULL;
}

bool interrupt_disable() {
  size_t flags = asm_get_flags();
  asm_cli;
  return (flags >> 9) & 1;
}

// 获得 IF 位
bool get_interrupt_state() {
  size_t flags = asm_get_flags();
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
    key->waiter = current_task;
    if (key->value != LOCK_UNLOCKED && key->waiter) {
      mtask_run_now(key->owner);
      if (current_task->ready == 1) { current_task->ready = 0; }
      task_fall_blocked(WAITING);
    }
  }
  interrupt_disable();
  key->waiter = NULL;
  key->value  = LOCK_LOCKED;
  key->owner  = current_task;
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
