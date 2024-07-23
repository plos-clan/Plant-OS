#include <kernel.h>
#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TIMERCTL timerctl;
void            fpu_disable();

#define TIMER_FLAGS_ALLOC 1 /* 已配置状态 */
#define TIMER_FLAGS_USING 2 /* 定时器运行中 */

extern int          cg_flag0;
extern struct TASK *c_task;

void init_pit() {
  asm_out8(0x43, 0x34);
  asm_out8(0x40, 0x9c);
  asm_out8(0x40, 0x2e);

  int           i;
  struct TIMER *t;
  timerctl.count = 0;
  for (i = 0; i < MAX_TIMER; i++) {
    timerctl.timers0[i].flags = 0; /* 没有使用 */
  }
  t           = timer_alloc(); /* 取得一个 */
  t->timeout  = 0xffffffff;
  t->flags    = TIMER_FLAGS_USING;
  t->next     = 0;            /* 末尾 */
  timerctl.t0 = t;            /* 因为现在只有哨兵，所以他就在最前面*/
  timerctl.next = 0xffffffff; /* 因为只有哨兵，所以下一个超时时刻就是哨兵的时刻 */
}

struct TIMER *timer_alloc() {
  int i;
  for (i = 0; i < MAX_TIMER; i++) {
    if (timerctl.timers0[i].flags == 0) {
      timerctl.timers0[i].flags  = TIMER_FLAGS_ALLOC;
      timerctl.timers0[i].waiter = NULL;
      return &timerctl.timers0[i];
    }
  }
  return 0; /* 没找到 */
}

void timer_free(struct TIMER *timer) {
  timer->flags  = 0; /* 未使用 */
  timer->waiter = NULL;
  return;
}

void timer_init(struct TIMER *timer, circular_queue_t queue, u8 data) {
  timer->queue = queue;
  timer->data  = data;
}

void timer_settime(struct TIMER *timer, u32 timeout) {
  int           e;
  struct TIMER *t, *s;
  timer->timeout = timeout + timerctl.count;
  timer->flags   = TIMER_FLAGS_USING;
  e              = io_load_eflags();
  t              = timerctl.t0;
  if (timer->timeout <= t->timeout) {
    /* 插入最前面的情况 */
    timerctl.t0   = timer;
    timer->next   = t; /* 下面是设定t */
    timerctl.next = timer->timeout;
    io_store_eflags(e);
    return;
  }
  while (true) {
    s = t;
    t = t->next;
    if (timer->timeout <= t->timeout) {
      /* 插入s和t之间的情况 */
      s->next     = timer; /* s下一个是timer */
      timer->next = t;     /* timer的下一个是t */
      io_store_eflags(e);
      return;
    }
  }
}

#define NANOSEC_IN_SEC 1000000000

void sleep(uint64_t time_s) {
  //   time_ns_t end_time;
  //   gettime_ns(&end_time);
  //   end_time.sec  = time_s / 1000;
  //   end_time.nsec = time_s % 1000 * 1000000;
  //   if (end_time.nsec > NANOSEC_IN_SEC) {
  //     end_time.sec  += 1;
  //     end_time.nsec -= NANOSEC_IN_SEC;
  //   }
  //   time_ns_t now_time;
  //   do {
  //     gettime_ns(&now_time);
  //   } while (now_time.sec < end_time.sec || now_time.nsec < end_time.nsec);
}

u32        mt2flag     = 0;
int        g           = 0;
static u32 count       = 0;
uint64_t   global_time = 0;
void       inthandler20(int cs, int *esp) {
  // printk("CS:EIP=%04x:%08x\n",current_task()->tss.cs,esp[-10]);
  asm_out8(PIC0_OCW2, 0x60); /* 把IRQ-00接收信号结束的信息通知给PIC */
  extern mtask *current;
  if (global_time + 1 == 0) {
    logk("reset\n");
    extern mtask m[255];
    for (int i = 0; i < 255; i++) {
      m[i].jiffies = 0;
    }
    global_time = 0;
  }
  global_time++;
  struct TIMER *timer;

  // gettime_ns(NULL); // 更新时间

  timerctl.count++;

  timer   = timerctl.t0; /* 首先把最前面的地址赋给timer */
  char ts = 0;
  while (true) {
    /* 因为timers的定时器都处于运行状态，所以不确认flags */
    if (timer->timeout > timerctl.count) { break; }
    /* 超时 */
    timer->flags = TIMER_FLAGS_ALLOC;
    task_run(timer->waiter);
    circular_queue_put(timer->queue, timer->data);
    timer = timer->next; /* 将下一个定时器的地址赋给timer*/
  }
  timerctl.t0   = timer;
  timerctl.next = timer->timeout;
  asm_sti;
  if (current) task_next();
}