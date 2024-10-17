#include <kernel.h>

// TODO 重构

int  DisableExpFlag = 0;
u32  CatchEIP       = 0;
char flagOfexp      = 0;
char public_catch   = 0;
int  st_task        = 0;

void SwitchPublic() {
  public_catch = 1;
}

void SwitchPrivate() {
  public_catch = 0;
}

void disableExp() {
  if (public_catch) {
    DisableExpFlag = 1;
  } else {
    current_task()->DisableExpFlag = 1;
  }
}

void EnableExp() {
  if (public_catch) {
    DisableExpFlag = 0;
  } else {
    current_task()->DisableExpFlag = 0;
  }
}

char GetExpFlag() {
  // printk("Get.\n");
  if (public_catch) {
    return flagOfexp;
  } else {
    return current_task()->flagOfexp;
  }
}

void ClearExpFlag() {
  if (public_catch) {
    flagOfexp = 0;
  } else {
    current_task()->flagOfexp = 0;
  }
}

void SetCatchEip(u32 eip) {
  // printk("eip = %08x\n",eip);
  if (public_catch) {
    CatchEIP = eip;
  } else {
    current_task()->CatchEIP = eip;
  }
}

void print_32bits_ascil(u32 n);

void insert_char(char *str, int pos, char ch) {
  int i;
  for (i = strlen(str); i >= pos; i--) {
    str[i + 1] = str[i];
  }
  str[pos] = ch;
}
void delete_char(char *str, int pos) {
  int i;
  for (i = pos; i < strlen(str); i++) {
    str[i] = str[i + 1];
  }
}

mtask *last_fpu_task = NULL;
void   fpu_disable() {
  asm_set_ts, asm_set_em;
}

void fpu_enable(mtask *task) {
  asm_clr_ts, asm_clr_em;
  if (!task->fpu_flag) {
    asm volatile("fnclex \n"
                 "fninit \n" ::
                     : "memory");
    memset(&task->fpu, 0, sizeof(fpu_regs_t));
    klogd("FPU create state for task 0x%08x\n", task);
  } else {
    asm volatile("frstor (%%eax) \n" ::"a"(&(task->fpu)) : "memory");
  }
  task->fpu_flag = 1;
}

finline void _ERROR_(int CODE, char *TIPS) {
  asm_cli;
  infinite_loop;
}

#define _ERROR(_code_, _tips_)                                                                     \
  void ERROR##_code_(u32 eip) {                                                                    \
    u32 *esp = &eip;                                                                               \
    _ERROR_(_code_, _tips_);                                                                       \
    if (public_catch) {                                                                            \
      *esp = CatchEIP;                                                                             \
    } else {                                                                                       \
      *esp = current_task()->CatchEIP;                                                             \
    }                                                                                              \
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

int dflag = 0;

bool has_fpu_error() {
  dflag = 0;
  u16 status_word;
  asm("fnstsw %0" : "=m"(status_word));
  dflag = 1;
  return (status_word & 0x1F) != 0;
}

void ERROR7(u32 eip) {
  if (current_task()->fpu_flag > 1 || current_task()->fpu_flag < 0) {
    asm_set_cr0(asm_get_cr0() & ~(CR0_EM | CR0_TS));
    return;
  }
  fpu_enable(current_task());
}
#define FP_TO_LINEAR(seg, off) ((u32)(seg << 4) + (u32)off)
#define VALID_FLAGS            0xDFF
#define EFLAG_VM               (1 << 17)
#define EFLAG_IF               (1 << 9)
void ERROR13(u32 eip) {
  if (current_task()->v86_mode == 0) {
    error("fault, gp");
    syscall_exit(1);
    infinite_loop;
  }
  volatile v86_frame_t *frame = (v86_frame_t *)((volatile u32)(&eip));
  asm volatile("" : : "g"(frame));
  // 打印frame所有成员
  uint8_t  *ip;
  uint16_t *stack, *ivt;
  uint32_t *stack32;
  mtask    *current = current_task();
  bool      is_operand32, is_address32;
  ip           = (uint8_t *)(FP_TO_LINEAR(frame->cs, frame->eip));
  stack        = (uint16_t *)(FP_TO_LINEAR(frame->ss, frame->esp));
  ivt          = (uint16_t *)(0x0);
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
