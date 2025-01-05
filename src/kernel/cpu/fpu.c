#include <kernel.h>

task_t fpu_using_task  = null;
bool   fpu_ctx_usermod = false; // 当前的 fpu 上下文是否是用户态的，否则是内核态的

void fpu_disable() {
  if (cpuids.fpu) asm_set_ts, asm_set_em;
  if (cpuids.sse) asm_clr_mp;
}

void fpu_enable() {
  if (cpuids.fpu) asm_clr_ts, asm_clr_em;
  if (cpuids.sse) asm_set_mp;
}

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
  bool em_seted = asm_get_cr0() & CR0_EM;
  if (em_seted) fpu_enable();
  fpu_do_save(extra_regs);
  if (em_seted) fpu_disable();
}

void fpu_init() {
  bool em_seted = asm_get_cr0() & CR0_EM;
  if (em_seted) fpu_enable();
  asm volatile("fnclex\n\tfninit\n\t" ::: "memory");
  if (em_seted) fpu_disable();
}

static void fpu_fix_ctx(task_t task, bool is_usermode) {
  kassert(task->fpu_enabled);
  fpu_enable();

  val old_regs =
      fpu_using_task == null ? null : //
          (fpu_ctx_usermod ? fpu_using_task->extra_regs : fpu_using_task->kernel_extra_regs);
  val new_regs = is_usermode ? task->extra_regs : task->kernel_extra_regs;

  if (old_regs == new_regs) return;

  if (old_regs) fpu_do_save(old_regs);

  if (cpuids.sse) {
    asm volatile("fxrstor (%0) \n" ::"r"(new_regs) : "memory");
  } else if (cpuids.fpu) {
    asm volatile("frstor (%0) \n" ::"r"(new_regs) : "memory");
  } else {
    fatal("FPU not supported");
  }

  fpu_using_task  = task;
  fpu_ctx_usermod = is_usermode;
}

FASTCALL void ERROR7(i32 id, regs32 *regs) {
  fpu_fix_ctx(current_task, regs->fs != RING0_DS);
}

FASTCALL void irq13(i32 id, regs32 *regs) {
  kloge("IRQ 13 should not be triggered");
  fpu_fix_ctx(current_task, regs->fs != RING0_DS);
}
