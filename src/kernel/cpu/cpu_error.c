#include <kernel.h>

// TODO 重构

void fpu_disable() {
  asm_set_ts, asm_set_em;
}

void fpu_enable(task_t task) {
  asm_clr_ts, asm_clr_em;
  if (!task->fpu_enabled) {
    task->fpu_enabled = true;
    asm volatile("fnclex \n"
                 "fninit \n" ::
                     : "memory");
    memset(&task->fpu, 0, sizeof(fpu_regs_t));
    klogd("FPU create state for task %p\n", task);
  } else {
    asm volatile("frstor (%0) \n" ::"r"(&(task->fpu)) : "memory");
  }
}

finline void _ERROR_(int CODE, char *TIPS) {
  klogf("%d: %s", CODE, TIPS);
  asm_cli;
  infinite_loop;
}

#define _ERROR(_code_, _tips_)                                                                     \
  void ERROR##_code_() {                                                                           \
    _ERROR_(_code_, _tips_);                                                                       \
  }

_ERROR(0, "#DE");
_ERROR(1, "#DB");
_ERROR(3, "#BP");
_ERROR(4, "#OF");
_ERROR(5, "#BR");
_ERROR(6, "#UD");
_ERROR(8, "#DF");
_ERROR(9, "#MF");
_ERROR(10, "#TS");
_ERROR(11, "#NP");
_ERROR(12, "#SS");
_ERROR(16, "#MF");
_ERROR(17, "#AC");
_ERROR(18, "#MC");
_ERROR(19, "#XF");

#undef _ERROR

void ERROR7(u32 eip) {
  if (current_task->fpu_enabled > 1 || current_task->fpu_enabled < 0) {
    asm_set_cr0(asm_get_cr0() & ~(CR0_EM | CR0_TS));
    return;
  }
  fpu_enable(current_task);
}

#define FP_TO_LINEAR(seg, off) ((u32)(seg << 4) + (u32)off)
#define VALID_FLAGS            0xDFF
#define EFLAG_VM               (1 << 17)
#define EFLAG_IF               (1 << 9)

extern byte *IVT;

void ERROR13(u32 eip) {
  volatile v86_frame_t *frame = (v86_frame_t *)((volatile u32)(&eip));
  if (current_task->v86_mode != 1) {
    error("fault, gp at 0x%x\n", frame->eip);
    syscall_exit(1);
    infinite_loop;
  }

  asm volatile("" : : "g"(frame));
  // 打印frame所有成员
  uint8_t      *ip;
  uint16_t     *stack, *ivt;
  uint32_t     *stack32;
  task_t        current = current_task;
  extern task_t v86_using_task;
  bool          is_operand32, is_address32;
  ip           = (uint8_t *)(FP_TO_LINEAR(frame->cs, frame->eip));
  stack        = (uint16_t *)(FP_TO_LINEAR(frame->ss, frame->esp));
  ivt          = (uint16_t *)(IVT);
  stack32      = (uint32_t *)(FP_TO_LINEAR(frame->ss, frame->esp));
  is_operand32 = is_address32 = false;
  while (true) {
    switch (ip[0]) {
    case 0x66: /* O32 */
      is_operand32 = true;
      ip++;
      frame->eip = (uint16_t)(frame->eip + 1);
      break;

    case 0x67: /* A32 */
      is_address32 = true;
      ip++;
      frame->eip = (uint16_t)(frame->eip + 1);
      break;

    case 0x9c: /* PUSHF */

      if (is_operand32) {
        frame->esp = ((frame->esp & 0xffff) - 4) & 0xffff;
        stack32--;
        stack32[0] = frame->eflags & VALID_FLAGS;

        if (current->v86_if)
          stack32[0] |= EFLAG_IF;
        else
          stack32[0] &= ~EFLAG_IF;
      } else {
        frame->esp = ((frame->esp & 0xffff) - 2) & 0xffff;
        stack--;
        stack[0] = (uint16_t)frame->eflags;

        if (current->v86_if)
          stack[0] |= EFLAG_IF;
        else
          stack[0] &= ~EFLAG_IF;
      }

      frame->eip = (uint16_t)(frame->eip + 1);
      return;

    case 0x9d: /* POPF */

      if (is_operand32) {
        frame->eflags   = EFLAG_IF | EFLAG_VM | (stack32[0] & VALID_FLAGS);
        current->v86_if = (stack32[0] & EFLAG_IF) != 0;
        frame->esp      = ((frame->esp & 0xffff) + 4) & 0xffff;
      } else {
        frame->eflags   = EFLAG_IF | EFLAG_VM | stack[0];
        current->v86_if = (stack[0] & EFLAG_IF) != 0;
        frame->esp      = ((frame->esp & 0xffff) + 2) & 0xffff;
      }

      frame->eip = (uint16_t)(frame->eip + 1);
      return;

    case 0xcd: /* INT n */
      switch (ip[1]) {
      case 0x30:
        // printf("int 0x30\n");
        frame->eip += 2;
        if (v86_using_task) { task_run(v86_using_task); }
        task_next();
        return;
      default:
        stack      -= 3;
        frame->esp  = ((frame->esp & 0xffff) - 6) & 0xffff;

        stack[0] = (uint16_t)(frame->eip + 2);
        stack[1] = frame->cs;
        stack[2] = (uint16_t)frame->eflags;

        if (current->v86_if)
          stack[2] |= EFLAG_IF;
        else
          stack[2] &= ~EFLAG_IF;
        frame->cs  = ivt[ip[1] * 2 + 1];
        frame->eip = ivt[ip[1] * 2];
        return;
      }

      break;

    case 0xcf: /* IRET */
      frame->eip      = stack[0];
      frame->cs       = stack[1];
      frame->eflags   = EFLAG_IF | EFLAG_VM | stack[2];
      current->v86_if = (stack[2] & EFLAG_IF) != 0;

      frame->esp = ((frame->esp & 0xffff) + 6) & 0xffff;
      return;

    case 0xfa: /* CLI */
      current->v86_if = false;
      frame->eip      = (uint16_t)(frame->eip + 1);
      return;

    case 0xfb: /* STI */
      current->v86_if = true;
      frame->eip      = (uint16_t)(frame->eip + 1);
      return;
    default: error("unhandled opcode 0x%x\n", ip[0]); return;
    }
  }
  infinite_loop;
}

void KILLAPP(int eip, int ec) {
  infinite_loop;
}

void KILLAPP0(int ec, int tn) {
  return;
}
