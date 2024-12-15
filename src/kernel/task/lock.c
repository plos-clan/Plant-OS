// This code is released under the MIT License

#include <kernel.h>

void lock_init(lock_t *l) {
  l->owner  = NULL;
  l->value  = LOCK_UNLOCKED;
  l->waiter = NULL;
}

void lock(lock_t *key) {
  with_no_interrupts({
    if (key->value != LOCK_UNLOCKED) {
      key->waiter = current_task;
      if (key->value != LOCK_UNLOCKED && key->waiter) {
        task_run(key->owner);
        task_fall_blocked(WAITING);
      }
    }
    key->waiter = NULL;
    key->value  = LOCK_LOCKED;
    key->owner  = current_task;
  });
}

void unlock(lock_t *key) {
  with_no_interrupts({
    key->value = LOCK_UNLOCKED;
    if (key->waiter) {
      task_run(key->waiter);
      task_run(key->waiter);
      key->waiter = NULL;
      task_next();
    }
  });
}
