#include <kernel.h>

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

struct TIMERCTL timerctl;

#define TIMER_FLAGS_ALLOC 1 /* 已配置状态 */
#define TIMER_FLAGS_USING 2 /* 定时器运行中 */

extern int    cg_flag0;
extern mtask *c_task;

void init_pit() {
  asm_out8(0x43, 0x34);
  asm_out8(0x40, 0x9c);
  asm_out8(0x40, 0x2e);
  timerctl.count = 0;
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

void inthandler20(int cs, int *esp) {
  send_eoi(0);
  klog_raw(".");
  extern mtask *mtask_current;
  if (global_time + 1 == 0) {
    klogd("reset\n");
    // extern mtask m[255];
    // for (int i = 0; i < 255; i++) {
    //   m[i].jiffies = 0;
    // }
    global_time = 0;
  }
  global_time++;
  struct TIMER *timer;

  // gettime_ns(NULL); // 更新时间

  timerctl.count++;

  asm_sti;
  if (mtask_current) task_next();
}
