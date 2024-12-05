#include <kernel.h>

#define PIT_CTRL 0x0043
#define PIT_CNT0 0x0040

u64 system_tick = 0;

static void pit_set(u16 value) {
  asm_out8(0x40, value & 0xff);
  asm_out8(0x40, value >> 8);
}

void init_pit() {
  asm_out8(0x43, 0x34);
  pit_set(1193182 / 100);
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

void inthandler20(int cs, int *esp) {
  send_eoi(0);
  extern mtask *mtask_current;

  // gettime_ns(NULL); // 更新时间

  system_tick++;

  asm_sti;
  if (mtask_current) task_next();
}
