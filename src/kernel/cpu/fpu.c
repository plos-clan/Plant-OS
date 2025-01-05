#include <kernel.h>

task_t fpu_using_task  = null;
bool   fpu_ctx_usermod = false; // 当前的 fpu 上下文是否是用户态的，否则是内核态的

void fpu_do_save(void *extra_regs) {
  if (cpuids.sse) {
    asm volatile("fxsave (%0)\n\t" ::"r"(extra_regs) : "memory");
  } else if (cpuids.fpu) {
    asm volatile("fnsave (%0)\n\t" ::"r"(extra_regs) : "memory");
  } else {
    fatal("FPU not supported");
  }
}

void fpu_save(void *extra_regs) {
  bool ts_seted = asm_get_cr0() & CR0_TS;
  if (ts_seted) asm_clr_ts;
  fpu_do_save(extra_regs);
  if (ts_seted) asm_set_ts;
}

void fpu_init() {
  bool ts_seted = asm_get_cr0() & CR0_TS;
  if (ts_seted) asm_clr_ts;
  if (cpuids.fpu) asm volatile("fnclex\n\tfninit\n\t");
  const u32 value = 0x1f80;
  if (cpuids.sse) asm volatile("ldmxcsr (%0)" ::"r"(&value));
  if (ts_seted) asm_set_ts;
}

static void fpu_fix_ctx(task_t task, bool is_usermode) {
  asm_clr_ts;

  val old_regs =
      fpu_using_task == null ? null : //
          (fpu_ctx_usermod ? fpu_using_task->extra_regs : fpu_using_task->kernel_extra_regs);
  val new_regs = is_usermode ? task->extra_regs : task->kernel_extra_regs;

  if (old_regs == new_regs) return;

  klogw("%d -> %d", fpu_using_task->tid, task->tid);
  klogw("%p -> %p", old_regs, new_regs);

  if (fpu_using_task && fpu_using_task->state != THREAD_DEAD) fpu_do_save(old_regs);

  if (fpu_using_task) task_unref(fpu_using_task);
  fpu_using_task  = task;
  fpu_ctx_usermod = is_usermode;
  task_ref(fpu_using_task);

  if (!task->fpu_enabled) {
    fpu_init();
    task->fpu_enabled = true;
    return;
  }

  if (cpuids.sse) {
    asm volatile("fxrstor (%0) \n" ::"r"(new_regs) : "memory");
  } else if (cpuids.fpu) {
    asm volatile("frstor (%0) \n" ::"r"(new_regs) : "memory");
  } else {
    fatal("FPU not supported");
  }

  asm volatile("fnclex\n\t");
}

FASTCALL void ERROR7(i32 id, regs32 *regs) {
  fpu_fix_ctx(current_task, regs->fs != RING0_DS);
}

FASTCALL void irq13(i32 id, regs32 *regs) {
  kloge("IRQ 13 should not be triggered");
  fpu_fix_ctx(current_task, regs->fs != RING0_DS);
}
