// This code is released under the MIT License

#include <kernel.h>

#pragma clang optimize off

#define STACK_SIZE (1024 * 1024)

void free_pde(u32 addr);

TSS32            tss;
task_t           idle_task;
task_t           mtask_current = null;
static avltree_t tasks;

task_t      next_set = null;
struct task empty_task;

task_t task_by_id(i32 tid) {
  if (tid < 0) return null;
  return avltree_get(tasks, tid);
}

static queue_t running_tasks;
static spin_t  running_tasks_lock;

finline void running_tasks_push(task_t task) {
  spin_lock(running_tasks_lock);
  if (running_tasks == null) running_tasks = queue_alloc();
  queue_enqueue(running_tasks, task);
  spin_unlock(running_tasks_lock);
}

finline task_t running_tasks_pop() {
  task_t task = null;
  spin_lock(running_tasks_lock);
  while (task == null) {
    task = queue_dequeue(running_tasks);
  }
  spin_unlock(running_tasks_lock);
  return task;
}

static void init_task(task_t t, int id) {
  t->jiffies          = 0;
  t->user_mode        = 0; // 此项暂时废除
  t->running          = 0;
  t->timeout          = 0;
  t->state            = EMPTY; // EMPTY
  t->tid              = id;    // task id
  t->ptid             = -1;    // parent task id
  t->keyboard_press   = NULL;  // keyboard hook
  t->keyboard_release = NULL;
  t->urgent           = 0;
  t->fpu_flag         = 0;
  t->fifosleep        = 0;
  t->mx               = 0;
  t->my               = 0;
  t->line             = NULL;
  t->timer            = NULL;
  t->waittid          = U32_MAX;
  t->alloc_addr       = 0;
  t->alloc_size       = 0;
  t->alloced          = 0;
  t->ready            = 0;
  t->pde              = 0;
  t->press_key_fifo   = NULL;
  t->release_keyfifo  = NULL;
  t->sigint_up        = 0;
  t->train            = 0;
  t->signal_disable   = 0;
  t->times            = 0;
  t->keyboard_press   = NULL;
  t->keyboard_release = NULL;
  t->keyfifo          = NULL;
  t->mousefifo        = NULL;
  t->signal           = 0;
  t->v86_mode         = 0;
  lock_init(&t->ipc_header.l);
  t->ipc_header.now = 0;
  for (int k = 0; k < MAX_IPC_MESSAGE; k++) {
    t->ipc_header.messages[k].from_tid = -1;
    t->ipc_header.messages[k].flag1    = 0;
    t->ipc_header.messages[k].flag2    = 0;
  }
  for (int k = 0; k < 30; k++) {
    t->handler[k] = 0;
  }
}

static void init_tasks() {}

fpu_regs_t public_fpu;

bool task_check_train(task_t task) {
  if (!task) return false;
  if (task->train == 1 && system_tick - task->jiffies >= 5) return true;
  return false;
}

extern task_t mouse_use_task;

void task_next() {
  if (!mtask_current) fatal("current is null");

  if (mtask_current->running < mtask_current->timeout - 1 && mtask_current->state == RUNNING &&
      next_set == NULL) {
    mtask_current->running++;
    return; // 不需要调度，当前时间片仍然属于你
  }
  asm_cli;

  if (!next_set) mtask_current->running = 0;

  task_t next = NULL;
  int    i;
  task_t j = NULL;

  if (next_set) {
    i        = next_set->tid;
    j        = next_set;
    next_set = NULL;
  } else {
    i = 0;
  }
  for (; i < 255; i++) {
    task_t p = task_by_id(i);
    // assert(p != NULL);
    if (p == NULL) { continue; }
    if (p == mtask_current) continue;
    if (p->state != RUNNING) { // RUNNING
      if (p->state == WAITING) {
        if (p->ready) {
          p->ready = 0;
          p->state = RUNNING;
          goto OK;
        }
        if (p->waittid == U32_MAX) continue;
      }
      // if (p->state == DIED) { p->state = EMPTY; }
      continue;
    }
  OK:
    if (p->urgent) {
      next = p;
      break;
    }
    if (!next || p->jiffies < next->jiffies || p->running) next = p;
  }
H:
  if (next == NULL) next = idle_task;
  if (next->user_mode == 1 || next->v86_mode == 1) {
    tss.esp0 = next->top;
    if (next->v86_mode == 1)
      tss.iomap = ((u32)offsetof(TSS32, io_map));
    else
      tss.iomap = 0;
  }
  if (next->urgent) next->urgent = 0;
  if (next->ready) next->ready = 0;
  int         current_fpu_flag = mtask_current->fpu_flag;
  fpu_regs_t *current_fpu      = &(mtask_current->fpu);
  asm_set_cr0(asm_get_cr0() & ~(CR0_EM | CR0_TS));
  if (current_fpu && current_fpu_flag)
    asm volatile("fnsave (%%eax) \n" ::"a"(current_fpu) : "memory");
  next->jiffies = system_tick;
  fpu_disable(); // 禁用fpu 如果使用FPU就会调用ERROR7
  asm_sti;
  task_switch(next); // 调度
}

finline int alloc_tid() {
  static i32 next_tid = 0;        // 下一个线程号
  if (next_tid < 0) next_tid = 1; // 0 号是主进程
  while (task_by_id(next_tid) != null) {
    next_tid++;
  }
  return next_tid++;
}

task_t create_task(void *func, u32 ticks, u32 floor) {
  task_t t = malloc(sizeof(*t));
  init_task(t, alloc_tid());

  if (t == null) return null;
  u32 esp_alloced = (u32)page_alloc(STACK_SIZE) + STACK_SIZE;
  change_page_task_id(t->tid, (void *)(esp_alloced - STACK_SIZE), STACK_SIZE);
  t->esp       = (stack_frame *)(esp_alloced - sizeof(stack_frame)); // switch用到的栈帧
  t->esp->eip  = (size_t)func;                                       // 设置跳转地址
  t->user_mode = 0;                                                  // 设置是否是user_mode
  if (mtask_current == NULL) {
    asm_cli;
    // 还没启用多任务
    t->pde   = PDE_ADDRESS; // 所以先用预设好的页表
    t->times = PDE_ADDRESS;
  } else {
    t->pde   = pde_clone(current_task()->pde); // 启用了就复制一个
    t->times = t->pde;
  }
  t->top     = esp_alloced; // r0的esp
  t->floor   = floor;
  t->running = 0;
  t->timeout = ticks;
  t->state   = RUNNING; // running
  t->TTY     = NULL;
  t->jiffies = 0;

  avltree_insert(tasks, t->tid, t);
  return t;
}

task_t get_task(u32 tid) {
  task_t task = task_by_id(tid);
  if (!task) return NULL;
  if (task->state == READY) return NULL;
  return task;
}

void task_to_user_mode(u32 eip, u32 esp) {
  u32 addr = (u32)mtask_current->top;

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
  tss.esp0                 = mtask_current->top;
  klogd("tid: %d\n", current_task()->tid);
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

void task_kill(u32 tid) {
  task_t task = task_by_id(tid);
  if (mouse_use_task == current_task()) mouse_sleep(&mdec);
  for (int i = 0; i < 255; i++) {
    task_t t = task_by_id(i);
    if (!t) continue;
    if (t->state == EMPTY || t->state == DIED) continue;
    if (t->tid == tid) continue;
    if (t->ptid >= 0 && t->ptid == tid) { task_kill(t->tid); }
  }
  asm_cli;
  if (get_task(tid) == current_task()) { asm_set_cr3(PDE_ADDRESS); }
  free_pde(task->pde);
  task_free_all_pages(tid); // 释放内存
  if (task->press_key_fifo) {
    page_free(task->press_key_fifo->buf, 4096);
    free(task->press_key_fifo);
  }
  if (task->release_keyfifo) {
    page_free(task->release_keyfifo->buf, 4096);
    free(task->release_keyfifo);
  }

  if (task->ptid != -1 && task_by_id(task->ptid)->waittid == tid) {
    task_by_id(task->ptid)->state = RUNNING;
  }
  asm_cli;
  avltree_delete_with(tasks, tid, free);
  asm_sti;
  if (get_task(tid) == current_task()) {
    while (1) {
      asm_sti;
      task_next();
    }
  }
}

task_t current_task() {
  if (mtask_current != NULL) return mtask_current;
  empty_task.tid = NULL_TID;
  return &empty_task;
}

static void idle_loop() {
  infinite_loop task_next();
}

void into_mtask() {
  init_tasks();
  asm_clr_em, asm_clr_ts;
  asm volatile("fninit");
  asm volatile("fnsave (%%eax) \n" ::"a"(&public_fpu));
  fpu_disable();
  SegmentDescriptor *gdt = (SegmentDescriptor *)GDT_ADDR;
  memset(&tss, 0, sizeof(tss));
  memset(tss.io_map, 0, sizeof(tss.io_map));
  tss.ss0 = 1 * 8;
  set_segmdesc(gdt + 103, sizeof(TSS32), (int)&tss, AR_TSS32);
  load_tr(103 * 8);
  idle_task = create_task(idle_loop, 1, 3);
  create_task(init, 5, 1);
  asm_set_em, asm_set_ts, asm_set_ne;
  task_start(task_by_id(0));
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
  // 加急一下
  task->urgent  = 1;
  task->ready   = 1;
  task->running = 0;
}

void task_fifo_sleep(task_t task) {
  task->fifosleep = 1;
}

cir_queue8_t task_get_mouse_fifo(task_t task) {
  return task->mousefifo;
}

void task_lock() {
  if (current_task()->ptid == -1) {
    return;
  } else {
    // for (int i = 0; i < 255; i++) {
    //   if (m[i].state == EMPTY || m[i].state == WILL_EMPTY || m[i].state == READY) continue;
    //   if (m[i].tid == get_tid(current_task())) continue;
    //   if ((m[i].tid == current_task()->ptid || m[i].ptid == current_task()->ptid) &&
    //       m[i].state == RUNNING) {
    //     m[i].state = WAITING; // WAITING
    //   }
    // }
  }
}

void task_unlock() {
  if (current_task()->ptid == -1) {
    return;
  } else {
    // for (int i = 0; i < 255; i++) {
    //   if (m[i].state == EMPTY || m[i].state == WILL_EMPTY || m[i].state == READY) continue;
    //   if (m[i].tid == get_tid(current_task())) continue;
    //   if ((m[i].tid == current_task()->ptid || m[i].ptid == current_task()->ptid) &&
    //       m[i].state == WAITING) {
    //     m[i].state = RUNNING; // RUNNING
    //   }
    // }
  }
}

u32 get_father_tid(task_t t) {
  if (t->ptid == -1) { return get_tid(t); }
  return get_father_tid(get_task(t->ptid));
}

void task_fall_blocked(enum STATE state) {
  if (current_task()->ready == 1) {
    current_task()->ready = 0;
    return;
  }
  current_task()->state = state;
  current_task()->ready = 0;
  task_next();
}

extern PageInfo *pages;

void task_exit(u32 status) {
  if (mouse_use_task == current_task()) { mouse_sleep(&mdec); }
  u32    tid  = current_task()->tid;
  task_t task = current_task();
  for (int i = 0; i < 255; i++) {
    task_t t = task_by_id(i);
    if (!t) continue;
    if (t->state == EMPTY || t->state == DIED) continue;
    if (t->tid == tid) continue;
    if (t->ptid == tid) { task_kill(t->tid); }
  }
  asm_cli;
  task->status = status;
  task->state  = DIED;
  asm_set_cr3(PDE_ADDRESS);
  free_pde(task->pde);
  task_free_all_pages(tid); // 释放内存
  if (task->press_key_fifo) {
    page_free(task->press_key_fifo->buf, 4096);
    free(task->press_key_fifo);
  }
  if (task->release_keyfifo) {
    page_free(task->release_keyfifo->buf, 4096);
    free(task->release_keyfifo);
  }
  for (int k = 0; k < 30; k++) {
    task->handler[k] = 0;
  }
  if (task->ptid != -1 && task_by_id(task->ptid)->waittid == tid) {
    task_run(task_by_id(task->ptid));
  }
  if (task->ptid == -1) {
    klogd("set empty");
    avltree_delete_with(tasks, tid, free);
    task_start(get_task(0));
    while (1) {
      asm_sti;
      task_next();
    }
    infinite_loop;
  }
  task->ptid = -1;
  asm_sti;
  while (1) {
    asm_sti;
    task_next();
  }
  infinite_loop;
}

int waittid(u32 tid) {
  task_t t = get_task(tid);
  if (!t) return -1;
  if (t->ptid != current_task()->tid) return -1;
  current_task()->waittid = tid;
  while (t->state != DIED && t->ptid == current_task()->tid) {
    task_fall_blocked(WAITING);
  }
  klogd("here %d %d %d", t->ptid, current_task()->tid, t->tid);
  u32 status = t->status;
  klogd("task exit with code %d\n", status);
  avltree_delete_with(tasks, tid, free);
  return status;
}

void mtask_run_now(task_t obj) {
  next_set = obj;
}

task_t mtask_get_free() {
  task_t t = page_malloc_one_no_mark();
  init_task(t, alloc_tid());
  return t;
}

// THE FUNCTION CAN ONLY BE CALLED IN USER MODE!!!!
void interrput_exit();

static void build_fork_stack(task_t task) {
  u32 addr              = task->top;
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
  task_t m = mtask_get_free();
  if (!m) { return -1; }
  klogd("get free %08x\n", m);
  klogd("current = %08x\n", get_tid(current_task()));
  bool state = interrupt_disable();
  int  tid   = 0;
  tid        = m->tid;
  memcpy(m, current_task(), sizeof(struct task));
  u32 stack = (u32)page_alloc(STACK_SIZE);
  change_page_task_id(tid, (void *)stack, STACK_SIZE);
  // u32 off = m->top - (u32)m->esp;
  memcpy((void *)stack, (void *)(m->top - STACK_SIZE), STACK_SIZE);
  klogd("s = %08x \n", m->top - STACK_SIZE);
  m->top = stack += STACK_SIZE;
  stack          += STACK_SIZE;
  m->esp          = (stack_frame *)stack;
  if (current_task()->press_key_fifo) {
    m->press_key_fifo = malloc(sizeof(struct event));
    memcpy(m->press_key_fifo, current_task()->press_key_fifo, sizeof(struct event));
    m->press_key_fifo->buf = page_alloc(4096);
    memcpy(m->press_key_fifo->buf, current_task()->press_key_fifo->buf, 4096);
  }
  if (current_task()->release_keyfifo) {
    m->release_keyfifo = malloc(sizeof(struct event));
    memcpy(m->release_keyfifo, current_task()->release_keyfifo, sizeof(struct event));
    m->release_keyfifo->buf = page_alloc(4096);
    memcpy(m->release_keyfifo->buf, current_task()->release_keyfifo->buf, 4096);
  }
  if (current_task()->keyfifo) {
    m->keyfifo = page_malloc_one();
    memcpy(m->keyfifo, current_task()->keyfifo, sizeof(struct event));
    m->keyfifo->buf = page_malloc_one();
    memcpy(m->keyfifo->buf, current_task()->keyfifo->buf, 4096);
  }
  if (current_task()->mousefifo) {
    m->mousefifo = page_malloc_one();
    memcpy(m->mousefifo, current_task()->mousefifo, sizeof(struct event));
    m->mousefifo->buf = page_malloc_one();
    memcpy(m->mousefifo->buf, current_task()->mousefifo->buf, 4096);
  }
  m->pde     = pde_clone(current_task()->pde);
  m->running = 0;
  m->jiffies = 0;
  m->timeout = 1;
  m->state   = RUNNING;
  m->ptid    = get_tid(current_task());
  m->tid     = tid;
  klogd("m->tid = %d\n", m->tid);
  tid = m->tid;
  klogd("BUILD FORK STACK\n");
  build_fork_stack(m);
  set_interrupt_state(state);
  return tid;
}
