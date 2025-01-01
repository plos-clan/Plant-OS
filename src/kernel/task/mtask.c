// This code is released under the MIT License

#include <kernel.h>

// 内核栈不应该超过 64K
#define STACK_SIZE (64 * 1024)

TSS32  tss;
task_t task_current = null;

static avltree_t tasks      = null;
static task_t    next_set   = null;
struct task      empty_task = {.tid = -1, .cr3 = PD_ADDRESS};

// --------------------------------------------------
//; 分配任务

task_t task_by_id(i32 tid) {
  if (tid < 0) return null;
  return avltree_get(tasks, tid);
}

finline int alloc_tid() {
  static i32 next_tid = 0;        // 下一个线程号
  if (next_tid < 0) next_tid = 1; // 0 号是主进程
  while (task_by_id(next_tid) != null) {
    next_tid++;
  }
  return next_tid++;
}

static task_t task_alloc() {
  task_t t = page_alloc(sizeof(*t));
  if (t == null) {
    kloge("task_alloc failed");
    return null;
  }
  i32 tid = alloc_tid();
  klogi("task_alloc %d", tid);
  t->jiffies          = 0;
  t->user_mode        = 0;
  t->running          = 0;
  t->timeout          = 0;
  t->state            = THREAD_IDLE;
  t->parent           = null;
  t->fpu_enabled      = false;
  t->fifosleep        = 0;
  t->command_line     = null;
  t->alloc_addr       = 0;
  t->alloc_size       = 0;
  t->cr3              = 0;
  t->press_key_fifo   = null;
  t->release_keyfifo  = null;
  t->sigint_up        = 0;
  t->keyboard_press   = null;
  t->keyboard_release = null;
  t->keyfifo          = null;
  t->mousefifo        = null;
  t->status           = INT_MAX;
  t->v86_mode         = 0;
  t->rc               = 1;
  t->waiting_list     = null;
  t->children         = null;
  t->is_switched      = false; //
                               //
  t->tid               = tid;  // task id
  t->ptid              = -1;   // parent task id
  t->signal            = 0;
  t->signal_mask       = U32_MAX;
  t->starttime.tv_sec  = 0;
  t->starttime.tv_nsec = 0;
  t->cputime.tv_sec    = 0;
  t->cputime.tv_nsec   = 0;
  return t;
}

static void task_free(task_t t) {
  if (t == null) {
    kloge("task_free null");
    return;
  }
  if (t->state != THREAD_DEAD) {
    kloge("task_free state != THREAD_DEAD");
    return;
  }
  klogi("task_free %d", t->tid);
  kassert(t->children == null);
  if (t->parent != null) avltree_delete(t->parent->children, t->tid);
  if (t == current_task) asm_set_cr3(PD_ADDRESS);
  pd_free(t->cr3);
  if (t->mousefifo) {
    page_free(t->mousefifo->buf, PAGE_SIZE);
    free(t->mousefifo);
  }
  if (t->keyfifo) {
    page_free(t->keyfifo->buf, PAGE_SIZE);
    free(t->keyfifo);
  }
  if (t->press_key_fifo) {
    page_free(t->press_key_fifo->buf, PAGE_SIZE);
    free(t->press_key_fifo);
  }
  if (t->release_keyfifo) {
    page_free(t->release_keyfifo->buf, PAGE_SIZE);
    free(t->release_keyfifo);
  }
  if (t->command_line) page_free(t->command_line, strlen(t->command_line) + 1);
  page_free(t, sizeof(*t));
}

finline task_t task_ref(task_t t) {
  if (t == null) {
    kloge("task_ref null");
    return null;
  }
  if (t->rc == 0) {
    kloge("task_ref rc == 0");
    return null;
  }
  u32 old_rc = atom_add(&t->rc, 1);
  kassert(old_rc != 0);
  return t;
}

finline void task_unref(task_t t) {
  if (t == null) {
    kloge("task_unref null");
    return;
  }
  if (t->rc == 0) {
    kloge("task_unref rc == 0");
    return;
  }
  u32 old_rc = atom_sub(&t->rc, 1);
  kassert(old_rc != 0);
  if (old_rc == 1) task_free(t);
}

// --------------------------------------------------
//; 运行队列

static struct queue running_tasks[8];

void running_tasks_push(task_t task) {
  if (task == null) {
    kloge("running_tasks_push null");
    return;
  }
  val is_sti = asm_is_sti;
  asm_cli;
  if (task->state == THREAD_DEAD) {
    kloge("running_tasks_push task %d THREAD_DEAD", task->tid);
    if (is_sti) asm_sti;
    return;
  }
  task_ref(task);
  task->state = THREAD_RUNNING;
  queue_enqueue(&running_tasks[cpuid_coreid], task);
  if (is_sti) asm_sti;
}

task_t running_tasks_pop() {
  val is_sti = asm_is_sti;
  asm_cli;
  task_t task = queue_dequeue(&running_tasks[cpuid_coreid]);
  while (task == null || task->state != THREAD_RUNNING) {
    if (task == null) {
      asm_sti, asm_hlt, asm_cli;
    } else {
      task_unref(task);
    }
    task = queue_dequeue(&running_tasks[cpuid_coreid]);
  }
  task_unref(task);
  if (is_sti) asm_sti;
  return task;
}

// --------------------------------------------------
//; 调度

extern void asm_task_switch(task_t current, task_t next) __attr(fastcall); // 切换任务
extern void asm_task_start(task_t current, task_t next) __attr(fastcall);  // 开始任务

finline void task_switch(task_t next) {
  var is_sti = asm_is_sti;
  asm_cli;
  val task = task_current;
  kassert(next != null, "Can't switch to null task");
  kassert(task != null, "Can't switch from null task");
  if (next == task) {
    klogw("task_switch next == current");
  } else {
    task_ref(next);
    task_unref(task);
    task_current = next;
    asm_task_switch(task, next);
    kassert(task->state == THREAD_RUNNING);
  }
  if (is_sti) asm_sti;
}

__attr(noreturn) finline void task_start(task_t task) {
  asm_cli;
  kassert(task != null, "task_start null");
  kassert(task_current == null);
  task_ref(task);
  task_current = task;
  asm_task_start(null, task);
  klogf("task_start error");
  abort();
}

// --------------------------------------------------

extern task_t mouse_use_task;

void task_tick() {
  val task = task_current;
  kassert(task != null);
  if (task->state == THREAD_RUNNING) {
    task->running++;
    if (task->running < task->timeout && next_set == null) return;
    task_next();
  }
}

void task_next() {
  asm_cli;

  val task = task_current;
  kassert(task != null);

  task->running = 0;

  if (!task->is_switched && task->state == THREAD_RUNNING) running_tasks_push(task);
  task->is_switched = false;

  val next = next_set && next_set->state == THREAD_RUNNING ? next_set : running_tasks_pop();
  if (next == next_set) {
    next->is_switched = true;
    next_set          = null;
  }

  if (next->user_mode || next->v86_mode) {
    tss.esp0  = next->stack_bottom;
    tss.iomap = next->v86_mode ? offsetof(TSS32, io_map) : 0;
    asm_wrmsr(IA32_SYSENTER_ESP, next->stack_bottom, 0);
  }

  next->jiffies = system_tick;

  if (next != task) {
    asm_clr_em, asm_clr_ts;
    if (task->fpu_enabled) {
      if (cpuids.sse) {
        asm volatile("fnsave (%0)\n\t" ::"r"(task->extra_regs) : "memory");
      } else {
        asm volatile("fnsave (%0)\n\t" ::"r"(task->extra_regs) : "memory");
      }
    }
    fpu_disable(); // 禁用fpu 如果使用FPU就会调用ERROR7
  }

  if (next != task) task_switch(next);

  asm_sti;
}

task_t create_task(void *entry, u32 ticks, u32 floor) {
  val t = task_alloc();

  if (t == null) return null;
  void *stack = page_alloc(STACK_SIZE);
  usize esp   = (usize)stack + STACK_SIZE;
  t->esp      = (stack_frame *)esp - 1; // switch用到的栈帧
  t->esp->eip = (usize)entry;           // 设置跳转地址

  if (task_current != null) {
    t->ptid   = task_current->tid;
    t->parent = task_current;
    avltree_insert(task_current->children, t->tid, t);
  }

  t->cr3          = pd_clone(current_task->cr3);
  t->stack_bottom = esp;
  t->floor        = floor;
  t->running      = 0;
  t->timeout      = ticks;
  t->jiffies      = 0;
  page_link_addr_pde(TASK_USER_PART_ADDR, t->cr3, (usize)t->_user_part);

  with_no_interrupts(avltree_insert(tasks, t->tid, t));
  return t;
}

void task_to_user_mode(u32 eip, u32 esp) {
  regs32 *iframe = (regs32 *)task_current->stack_bottom - 1;

  iframe->edi = 0;
  iframe->esi = 0;
  iframe->ebp = 0;
  iframe->ebx = 0;
  iframe->edx = 0;
  iframe->ecx = 0;
  iframe->eax = 0;

  iframe->gs    = 0;
  iframe->ds    = GET_SEL(RING3_DS, SA_RPL3);
  iframe->es    = GET_SEL(RING3_DS, SA_RPL3);
  iframe->fs    = GET_SEL(RING3_DS, SA_RPL3);
  iframe->ss    = GET_SEL(RING3_DS, SA_RPL3);
  iframe->cs    = GET_SEL(RING3_CS, SA_RPL3);
  iframe->eip   = eip;
  iframe->flags = (0 << 12 | 0b10 | 1 << 9);
  iframe->esp   = esp; // 设置用户态堆栈

  task_current->user_mode = true;
  tss.esp0                = task_current->stack_bottom;

  asm volatile("mov %0, %%esp\n\t" ::"r"(iframe));
  asm volatile("jmp asm_inthandler_quit\n\t");
  __builtin_unreachable();
}

task_t get_current_task() {
  return task_current ?: &empty_task;
}

void into_mtask() {
  SegmentDescriptor *gdt = (SegmentDescriptor *)GDT_ADDR;
  memset(&tss, 0, sizeof(tss));
  tss.ss0 = RING0_DS;
  set_segmdesc(gdt + 103, sizeof(TSS32), (u32)&tss, AR_TSS32);
  load_tr(103 * 8);
  val task = task_run(create_task(&user_init, 5, 1));
  asm_set_ts, asm_set_em, asm_set_ne;
  task_start(task);
}

void task_set_fifo(task_t task, cir_queue8_t kfifo, cir_queue8_t mfifo) {
  task->keyfifo   = kfifo;
  task->mousefifo = mfifo;
}

cir_queue8_t task_get_key_queue(task_t task) {
  return task->keyfifo;
}

task_t task_run(task_t task) {
  if (task == null || task->status == THREAD_DEAD || task->status == THREAD_STOPPED) return task;
  if (task->state != THREAD_RUNNING) running_tasks_push(task);
  next_set = task;
  return task;
}

cir_queue8_t task_get_mouse_fifo(task_t task) {
  return task->mousefifo;
}

u32 get_father_tid(task_t t) {
  if (t->ptid == -1) { return t->tid; }
  return get_father_tid(task_by_id(t->ptid));
}

void task_fall_blocked() {
  klogd("task %d fall blocked", current_task->tid);
  current_task->state = THREAD_WAITING;
  task_next();
}

extern PageInfo *pages;

void task_kill(task_t task) {
  kassert(task != null);

  if (task->state == THREAD_IDLE) {}

  val tid = task->tid;

  if (mouse_use_task == task) mouse_sleep(&mdec);

  task->state = THREAD_DEAD;

  avltree_free_with(task->children, (free_t)task_kill);

  with_no_interrupts({
    list_foreach(task->waiting_list, node) {
      task_run(node->data);
    }
    list_free_with(task->waiting_list, (free_t)task_unref);
    avltree_delete_with(tasks, tid, (free_t)task_unref);
  });

  if (task != current_task) return;

  infinite_loop {
    asm_sti;
    task_next();
    klogf("task_kill error");
  }
}

void task_exit(i32 status) {
  val task = current_task;
  kassert(task != null);

  if (status < 0) klogw("task_exit status < 0");
  task->status = status & I32_MAX;

  task_kill(task);

  klogf("task_exit error");
  abort();
}

void task_abort() {
  val task = current_task;
  kassert(task != null);

  task_kill(task);

  klogf("task_abort error");
  abort();
}

i32 task_wait(task_t target) {
  kassert(task_current != null);
  if (target == null) return -1;
  task_ref(target);

  task_ref(task_current);
  list_prepend(target->waiting_list, task_current);

  while (target->state != THREAD_DEAD) {
    task_fall_blocked();
  }

  i32 status = target->status & I32_MAX;
  klogd("task %d exit with code %d", target->tid, status);

  task_unref(target);
  return status;
}

i32 waittid(i32 tid) {
  return task_wait(task_by_id(tid));
}

static void build_fork_stack(task_t task) {
  regs32 *iframe = (regs32 *)task->stack_bottom - 1;
  iframe->eax    = 0;
  klogd("iframe = %08x\n", iframe->eip);
  stack_frame *sframe = (stack_frame *)iframe - 1;
  sframe->ebp         = 0;
  sframe->ebx         = 0;
  sframe->ecx         = 0;
  sframe->edx         = 0;
  sframe->eip         = (size_t)asm_inthandler_quit;

  task->esp = sframe;
}

int task_fork() {
  val m = task_alloc();
  if (m == null) return -1;
  klogd("get free %08x\n", m);
  klogd("current = %08x\n", current_task->tid);
  int tid = m->tid;
  with_no_interrupts({
    memcpy(m, current_task, sizeof(struct task));
    u32 stack = (u32)page_alloc(STACK_SIZE);
    memcpy((void *)stack, (void *)(m->stack_bottom - STACK_SIZE), STACK_SIZE);
    klogd("s = %08x \n", m->stack_bottom - STACK_SIZE);
    m->stack_bottom = stack += STACK_SIZE;
    stack                   += STACK_SIZE;
    m->esp                   = (stack_frame *)stack;
    if (current_task->press_key_fifo) {
      m->press_key_fifo = malloc(sizeof(struct event));
      memcpy(m->press_key_fifo, current_task->press_key_fifo, sizeof(struct event));
      m->press_key_fifo->buf = page_alloc(4096);
      memcpy(m->press_key_fifo->buf, current_task->press_key_fifo->buf, 4096);
    }
    if (current_task->release_keyfifo) {
      m->release_keyfifo = malloc(sizeof(struct event));
      memcpy(m->release_keyfifo, current_task->release_keyfifo, sizeof(struct event));
      m->release_keyfifo->buf = page_alloc(4096);
      memcpy(m->release_keyfifo->buf, current_task->release_keyfifo->buf, 4096);
    }
    if (current_task->keyfifo) {
      m->keyfifo = page_malloc_one();
      memcpy(m->keyfifo, current_task->keyfifo, sizeof(struct event));
      m->keyfifo->buf = page_malloc_one();
      memcpy(m->keyfifo->buf, current_task->keyfifo->buf, 4096);
    }
    if (current_task->mousefifo) {
      m->mousefifo = page_malloc_one();
      memcpy(m->mousefifo, current_task->mousefifo, sizeof(struct event));
      m->mousefifo->buf = page_malloc_one();
      memcpy(m->mousefifo->buf, current_task->mousefifo->buf, 4096);
    }
    m->cr3     = pd_clone(current_task->cr3);
    m->running = 0;
    m->jiffies = 0;
    m->timeout = 1;
    m->state   = THREAD_RUNNING;
    m->ptid    = current_task->tid;
    m->tid     = tid;
    klogd("m->tid = %d\n", m->tid);
    tid = m->tid;
    klogd("BUILD FORK STACK\n");
    build_fork_stack(m);
  });
  return tid;
}
