#include <kernel.h>

#define PIT_CTRL 0x43
#define PIT_CNT0 0x40

volatile u64 system_tick = 0;

static void pit_set(u16 value) {
  asm_out8(PIT_CNT0, value & 0xff);
  asm_out8(PIT_CNT0, value >> 8);
}

static inthandler_f inthandler20;

#define PIT_FREQ 100

void init_pit() {
  inthandler_set(0x20, inthandler20);
  asm_out8(PIT_CTRL, 0x34);
  pit_set(1193182 / PIT_FREQ);
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

static void inthandler20(i32 id, regs32 *regs) {
  // gettime_ns(NULL); // 更新时间

  system_tick++;

  kenel_debugger_tick();

  extern task_t task_current;
  if (task_current) task_tick();
}
