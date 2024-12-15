// This code is released under the MIT License

#include <kernel.h>

// 内核栈不应该超过 64K
#define STACK_SIZE (64 * 1024)

void free_pde(u32 addr);

TSS32            tss;
task_t           mtask_current = null;
static avltree_t tasks         = null;

task_t      next_set   = null;
struct task empty_task = {.tid = -1, .pde = PDE_ADDRESS};

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
  task_t t = malloc(sizeof(*t));
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
  t->state            = EMPTY; // EMPTY
  t->tid              = tid;   // task id
  t->ptid             = -1;    // parent task id
  t->keyboard_press   = null;  // keyboard hook
  t->keyboard_release = null;
  t->urgent           = 0;
  t->fpu_enabled      = 0;
  t->fifosleep        = 0;
  t->command_line     = null;
  t->waittid          = -1;
  t->alloc_addr       = 0;
  t->alloc_size       = 0;
  t->alloced          = 0;
  t->pde              = 0;
  t->press_key_fifo   = null;
  t->release_keyfifo  = null;
  t->sigint_up        = 0;
  t->signal_disable   = 1; // 目前暂时关闭信号
  t->keyboard_press   = null;
  t->keyboard_release = null;
  t->keyfifo          = null;
  t->mousefifo        = null;
  t->signal           = 0;
  t->v86_mode         = 0;

  t->rc           = 1;
  t->waiting_list = null;
  t->children     = null;
  return t;
}

static void task_free(task_t t) {
  if (t == null) {
    kloge("task_free null");
    return;
  }
  if (t->state != DIED) {
    kloge("task_free state != DIED");
    return;
  }
  klogi("task_free %d", t->tid);
  if (t == current_task) asm_set_cr3(PDE_ADDRESS);
  free_pde(t->pde);
  task_free_all_pages(t->tid); // 释放内存
  if (t->press_key_fifo) {
    page_free(t->press_key_fifo->buf, 4096);
    free(t->press_key_fifo);
  }
  if (t->release_keyfifo) {
    page_free(t->release_keyfifo->buf, 4096);
    free(t->release_keyfifo);
  }
}

static task_t task_ref(task_t t) {
  if (t == null) {
    kloge("task_ref null");
    return null;
  }
  t->rc++;
  return t;
}

static void task_unref(task_t t) {
  if (t == null) {
    kloge("task_unref null");
    return;
  }
  if (t->rc == 0) {
    kloge("task_unref rc == 0");
    return;
  }
  if (--t->rc == 0) task_free(t);
}

// --------------------------------------------------

static struct queue running_tasks[8];

void running_tasks_push(task_t task) {
  if (task == null) {
    kloge("running_tasks_push null");
    return;
  }
  task_ref(task);
  with_no_interrupts(queue_enqueue(&running_tasks[cpuid_coreid], task));
}

task_t running_tasks_pop() {
  task_t task = null;
  with_no_interrupts(task = queue_dequeue(&running_tasks[cpuid_coreid]));
  while (task == null || task->state != RUNNING) {
    if (task == null) {
      asm_hlt;
    } else {
      task_unref(task);
    }
    with_no_interrupts(task = queue_dequeue(&running_tasks[cpuid_coreid]));
  }
  task_unref(task);
  return task;
}

// --------------------------------------------------

static fpu_regs_t public_fpu;

extern task_t mouse_use_task;

void task_tick() {
  // const var task = mtask_current;
  // assert(task != null);
  // task->running++;
  task_next();
}

void task_next() {
  const var task = mtask_current;
  assert(task != null);

  if (task->running < task->timeout && task->state == RUNNING && next_set == null) {
    task->running++; // why?
    return;
  }

  asm_cli;

  task->running = 0;

  running_tasks_push(task);
  task_t next = next_set && next_set->state == RUNNING ? next_set : running_tasks_pop();

  if (next->user_mode == 1 || next->v86_mode == 1) {
    tss.esp0 = next->stack_bottom;
    if (next->v86_mode == 1)
      tss.iomap = offsetof(TSS32, io_map);
    else
      tss.iomap = 0;
  }

  next->urgent = 0;

  asm_clr_em, asm_clr_ts;
  if (task->fpu_enabled) asm volatile("fnsave (%0)\n\t" ::"r"(&task->fpu) : "memory");
  next->jiffies = system_tick;
  fpu_disable(); // 禁用fpu 如果使用FPU就会调用ERROR7

  asm_sti;

  task_switch(next); // 调度
}

task_t create_task(void *entry, u32 ticks, u32 floor) {
  const var t = task_alloc();

  if (t == null) return null;
  u32 esp_alloced = (u32)page_alloc(STACK_SIZE) + STACK_SIZE;
  change_page_task_id(t->tid, (void *)(esp_alloced - STACK_SIZE), STACK_SIZE);
  t->esp       = (stack_frame *)(esp_alloced - sizeof(stack_frame)); // switch用到的栈帧
  t->esp->eip  = (size_t)entry;                                      // 设置跳转地址
  t->user_mode = 0;                                                  // 设置是否是user_mode

  t->pde          = pde_clone(current_task->pde); // 启用了就复制一个
  t->stack_bottom = esp_alloced;                  // r0的esp
  t->floor        = floor;
  t->running      = 0;
  t->timeout      = ticks;
  t->state        = RUNNING; // running
  t->jiffies      = 0;

  avltree_insert(tasks, t->tid, t);
  running_tasks_push(t);
  return t;
}

task_t get_task(u32 tid) {
  task_t task = task_by_id(tid);
  if (!task) return null;
  if (task->state == READY) return null;
  return task;
}

void task_to_user_mode(u32 eip, u32 esp) {
  u32 addr = (u32)mtask_current->stack_bottom;

  addr                 -= sizeof(intr_frame_t);
  intr_frame_t *iframe  = (intr_frame_t *)(addr);

  iframe->edi       = 0;
  iframe->esi       = 0;
  iframe->ebp       = 0;
  iframe->esp_dummy = 0;
  iframe->ebx       = 0;
  iframe->edx       = 0;
  iframe->ecx       = 0;
  iframe->eax       = 0;

  iframe->gs               = 0;
  iframe->ds               = GET_SEL(3 * 8, SA_RPL3);
  iframe->es               = GET_SEL(3 * 8, SA_RPL3);
  iframe->fs               = GET_SEL(3 * 8, SA_RPL3);
  iframe->ss               = GET_SEL(3 * 8, SA_RPL3);
  iframe->cs               = GET_SEL(4 * 8, SA_RPL3);
  iframe->eip              = eip;
  iframe->eflags           = (0 << 12 | 0b10 | 1 << 9);
  iframe->esp              = esp; // 设置用户态堆栈
  mtask_current->user_mode = 1;
  tss.esp0                 = mtask_current->stack_bottom;
  klogd("tid: %d\n", current_task->tid);
  asm_cli;
  asm volatile("movl %0, %%esp\n\t"
               "popa\n\t"
               "pop %%gs\n\t"
               "pop %%fs\n\t"
               "pop %%es\n\t"
               "pop %%ds\n\t"
               "iret\n\t" ::"m"(iframe)
               : "memory");
  __builtin_unreachable();
}

task_t get_current_task() {
  return mtask_current ?: &empty_task;
}

static void idle_loop() {
  infinite_loop task_next();
}

void into_mtask() {
  asm_clr_em, asm_clr_ts;
  asm volatile("fninit");
  asm volatile("fnsave (%%eax) \n" ::"a"(&public_fpu));
  fpu_disable();
  SegmentDescriptor *gdt = (SegmentDescriptor *)GDT_ADDR;
  memset(&tss, 0, sizeof(tss));
  tss.ss0 = 1 * 8;
  set_segmdesc(gdt + 103, sizeof(TSS32), (u32)&tss, AR_TSS32);
  load_tr(103 * 8);
  const var task = create_task(user_init, 5, 1);
  asm_set_em, asm_set_ts, asm_set_ne;
  task_start(task);
}

void task_set_fifo(task_t task, cir_queue8_t kfifo, cir_queue8_t mfifo) {
  task->keyfifo   = kfifo;
  task->mousefifo = mfifo;
}

cir_queue8_t task_get_key_queue(task_t task) {
  return task->keyfifo;
}

void task_sleep(task_t task) {
  task->state     = SLEEPING;
  task->fifosleep = 1;
}

void task_wake_up(task_t task) {
  task->state     = RUNNING;
  task->fifosleep = 0;
}

void task_run(task_t task) {
  if (task == null || task->status == DIED) return;
  task->state   = RUNNING;
  task->urgent  = 1; // 加急一下
  task->running = 0;
  running_tasks_push(task);
}

cir_queue8_t task_get_mouse_fifo(task_t task) {
  return task->mousefifo;
}

u32 get_father_tid(task_t t) {
  if (t->ptid == -1) { return t->tid; }
  return get_father_tid(get_task(t->ptid));
}

void task_fall_blocked(enum STATE state) {
  klogd("fall blocked %d\n", state);
  current_task->state = state;
  task_next();
}

extern PageInfo *pages;

void task_kill(task_t task) {
  assert(task != null);
  const var tid = task->tid;

  if (mouse_use_task == task) mouse_sleep(&mdec);

  task->state = DIED;

  avltree_free_with(task->children, (free_t)task_kill);

  with_no_interrupts({
    if (task->ptid != -1 && task_by_id(task->ptid)->waittid == tid) {
      task_run(task_by_id(task->ptid));
    }
    avltree_delete_with(tasks, tid, (free_t)task_unref);
  });

  if (task != current_task) return;

  infinite_loop {
    asm_sti;
    task_next();
  }
}

void task_exit(i32 status) {
  const var task = current_task;
  assert(task != null);

  task->status = status & I32_MAX;

  task_kill(task);

  klogf("task_exit error");
  abort();
}

i32 waittid(i32 tid) {
  task_t target = get_task(tid);
  if (target == null || target->ptid != current_task->tid) return -1;
  task_ref(target);
  // list_prepend(target->waiting_list, current_task);
  current_task->waittid = tid;
  while (target->state != DIED && target->ptid == current_task->tid) {
    task_fall_blocked(WAITING);
  }
  i32 status = target->status & I32_MAX;
  klogd("task %d exit with code %d\n", tid, status);
  task_unref(target);
  return status;
}

void mtask_run_now(task_t obj) {
  next_set = obj;
}

// THE FUNCTION CAN ONLY BE CALLED IN USER MODE!!!!
void interrput_exit();

static void build_fork_stack(task_t task) {
  u32 addr              = task->stack_bottom;
  addr                 -= sizeof(intr_frame_t);
  intr_frame_t *iframe  = (intr_frame_t *)addr;
  iframe->eax           = 0;
  klogd("iframe = %08x\n", iframe->eip);
  addr                -= sizeof(stack_frame);
  stack_frame *sframe  = (stack_frame *)addr;
  sframe->ebp          = 0;
  sframe->ebx          = 0;
  sframe->ecx          = 0;
  sframe->edx          = 0;
  sframe->eip          = (size_t)interrput_exit;

  task->esp = sframe;
}

int task_fork() {
  const var m = task_alloc();
  if (m == null) return -1;
  klogd("get free %08x\n", m);
  klogd("current = %08x\n", current_task->tid);
  int tid = m->tid;
  with_no_interrupts({
    memcpy(m, current_task, sizeof(struct task));
    u32 stack = (u32)page_alloc(STACK_SIZE);
    change_page_task_id(tid, (void *)stack, STACK_SIZE);
    // u32 off = m->top - (u32)m->esp;
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
    m->pde     = pde_clone(current_task->pde);
    m->running = 0;
    m->jiffies = 0;
    m->timeout = 1;
    m->state   = RUNNING;
    m->ptid    = current_task->tid;
    m->tid     = tid;
    klogd("m->tid = %d\n", m->tid);
    tid = m->tid;
    klogd("BUILD FORK STACK\n");
    build_fork_stack(m);
  });
  return tid;
}
