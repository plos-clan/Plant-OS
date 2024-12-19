#include <kernel.h>

static inthandler_f error_inthandler;
static inthandler_f ERROR7;
static inthandler_f ERROR13;
static inthandler_f irq13;

void fpu_disable() {
  asm_set_ts, asm_set_em;
}

void fpu_enable(task_t task) {
  asm_clr_ts, asm_clr_em;
  if (!task->fpu_enabled) {
    task->fpu_enabled = true;
    if (cpuids.sse) {
      asm volatile("fnclex\n\tfninit\n\t" ::: "memory");
    } else {
      asm volatile("fnclex\n\tfninit\n\t" ::: "memory");
    }
    memset(task->extra_regs, 0, sizeof(task->extra_regs));
    klogd("FPU create state for task %p\n", task);
  } else {
    if (cpuids.sse) {
      asm volatile("fxrstor (%0) \n" ::"r"(task->extra_regs) : "memory");
    } else {
      asm volatile("frstor (%0) \n" ::"r"(task->extra_regs) : "memory");
    }
  }
}

// View https://wiki.osdev.org/Exceptions for more information

static const struct {
  cstr shortname;
  cstr fullname;
} error_names[32] = {
    [0]  = {"DE", "Divide Error"                  },
    [1]  = {"DB", "Debug Exception"               },
    [2]  = {"--", "NMI Interrupt"                 },
    [3]  = {"BP", "Breakpoint"                    },
    [4]  = {"OF", "Overflow"                      },
    [5]  = {"BR", "BOUND Range Exceeded"          },
    [6]  = {"UD", "Invalid Opcode"                },
    [7]  = {"NM", "Device Not Available"          },
    [8]  = {"DF", "Double Fault"                  },
    [9]  = {"--", "Coprocessor Segment Overrun"   },
    [10] = {"TS", "Invalid TSS"                   },
    [11] = {"NP", "Segment Not Present"           },
    [12] = {"SS", "Stack-Segment Fault"           },
    [13] = {"GP", "General Protection"            },
    [14] = {"PF", "Page Fault"                    },
    [15] = {"--", "Reserved"                      },
    [16] = {"MF", "x87 FPU Floating-Point Error"  },
    [17] = {"AC", "Alignment Check"               },
    [18] = {"MC", "Machine Check"                 },
    [19] = {"XF", "SIMD Floating-Point Exception" },
    [20] = {"VE", "Virtualization Exception"      },
    [21] = {"CP", "Control Protection Exception"  },
    [22] = {"--", "Reserved"                      },
    [23] = {"--", "Reserved"                      },
    [24] = {"--", "Reserved"                      },
    [25] = {"--", "Reserved"                      },
    [26] = {"--", "Reserved"                      },
    [27] = {"--", "Reserved"                      },
    [28] = {"HV", "Hypervisor Injection Exception"},
    [29] = {"VC", "VMM Communication Exception"   },
    [30] = {"SX", "Security Exception"            },
    [31] = {"--", "Reserved"                      },
};

__attr(fastcall) void error_inthandler(i32 id, regs32 *regs) {
  kassert(id >= 0);
  kassert(id < 32);
  kassert(regs != null);
  if (regs->fs != RING0_DS) {
    klogw("task %d error %02x: %s (#%s)", current_task->tid, id, error_names[id].fullname,
          error_names[id].shortname);
    task_abort();
    __builtin_unreachable();
  }
  klogf("error %02x: %s (#%s)", id, error_names[id].fullname, error_names[id].shortname);
  abort();
}

__attr(fastcall) void ERROR7(i32 id, regs32 *reg) {
  fpu_enable(current_task);
}

__attr(fastcall) void irq13(i32 id, regs32 *reg) {
  kloge("IRQ 13 should not be triggered");
  fpu_enable(current_task);
}

void init_error_inthandler() {
  for (int i = 0; i < 32; i++) {
    inthandler_set(i, error_inthandler);
  }
  inthandler_set(0x07, ERROR7);
  inthandler_set(0x0d, ERROR13);
  inthandler_set(0x2d, irq13);
}

#define FP_TO_LINEAR(seg, off) ((u32)(seg << 4) + (u32)off)
#define VALID_FLAGS            0xDFF
#define EFLAG_VM               (1 << 17)
#define EFLAG_IF               (1 << 9)

extern byte *IVT;

extern task_t v86_using_task;

__attr(fastcall) void ERROR13(i32 id, regs32 *regs) {
  const var frame = regs;
  const var task  = current_task;
  if (current_task->v86_mode != 1) {
    error("fault, gp at 0x%x\n", frame->eip);
    syscall_exit(1);
    infinite_loop;
  }

  u8  *ip           = (u8 *)FP_TO_LINEAR(frame->cs, frame->eip);
  u16 *stack        = (u16 *)FP_TO_LINEAR(frame->ss, frame->esp);
  u16 *ivt          = (u16 *)IVT;
  u32 *stack32      = (u32 *)FP_TO_LINEAR(frame->ss, frame->esp);
  bool is_operand32 = false;
  bool is_address32 = false;
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
        stack32[0] = frame->flags & VALID_FLAGS;

        if (task->v86_if)
          stack32[0] |= EFLAG_IF;
        else
          stack32[0] &= ~EFLAG_IF;
      } else {
        frame->esp = ((frame->esp & 0xffff) - 2) & 0xffff;
        stack--;
        stack[0] = (uint16_t)frame->flags;

        if (task->v86_if)
          stack[0] |= EFLAG_IF;
        else
          stack[0] &= ~EFLAG_IF;
      }

      frame->eip = (uint16_t)(frame->eip + 1);
      return;

    case 0x9d: /* POPF */

      if (is_operand32) {
        frame->flags = EFLAG_IF | EFLAG_VM | (stack32[0] & VALID_FLAGS);
        task->v86_if = (stack32[0] & EFLAG_IF) != 0;
        frame->esp   = ((frame->esp & 0xffff) + 4) & 0xffff;
      } else {
        frame->flags = EFLAG_IF | EFLAG_VM | stack[0];
        task->v86_if = (stack[0] & EFLAG_IF) != 0;
        frame->esp   = ((frame->esp & 0xffff) + 2) & 0xffff;
      }

      frame->eip = (uint16_t)(frame->eip + 1);
      return;

    case 0xcd: /* INT n */
      switch (ip[1]) {
      case 0x30:
        frame->eip += 2;
        if (v86_using_task) task_run(v86_using_task);
        task_next();
        return;
      default:
        stack      -= 3;
        frame->esp  = ((frame->esp & 0xffff) - 6) & 0xffff;

        stack[0] = (uint16_t)(frame->eip + 2);
        stack[1] = frame->cs;
        stack[2] = (uint16_t)frame->flags;

        if (task->v86_if)
          stack[2] |= EFLAG_IF;
        else
          stack[2] &= ~EFLAG_IF;
        frame->cs  = ivt[ip[1] * 2 + 1];
        frame->eip = ivt[ip[1] * 2];
        return;
      }
      return;

    case 0xcf: /* IRET */
      frame->eip   = stack[0];
      frame->cs    = stack[1];
      frame->flags = EFLAG_IF | EFLAG_VM | stack[2];
      task->v86_if = (stack[2] & EFLAG_IF) != 0;

      frame->esp = ((frame->esp & 0xffff) + 6) & 0xffff;
      return;

    case 0xfa: /* CLI */
      task->v86_if = false;
      frame->eip   = (uint16_t)(frame->eip + 1);
      return;

    case 0xfb: /* STI */
      task->v86_if = true;
      frame->eip   = (uint16_t)(frame->eip + 1);
      return;
    default: error("unhandled opcode 0x%x\n", ip[0]); return;
    }
  }
}
