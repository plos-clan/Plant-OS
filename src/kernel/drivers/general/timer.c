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
  irq_enable(0);
}

void sleep(u64 time_s) {
  timespec end_time;
  gettime_ns(&end_time);
  end_time.tv_sec  += time_s / 1000;
  end_time.tv_nsec += time_s % 1000 * 1000000;
  if (end_time.tv_nsec > NANOSEC_IN_SEC) {
    end_time.tv_sec  += 1;
    end_time.tv_nsec -= NANOSEC_IN_SEC;
  }
  timespec now_time;
  do {
    gettime_ns(&now_time);
  } while (now_time.tv_sec < end_time.tv_sec || now_time.tv_nsec < end_time.tv_nsec);
}

struct timespec calc_time_by_tick(u64 tick) {
  const u64 ticktime_ns = NANOSEC_IN_SEC * (u64)(1193182 / PIT_FREQ) / 1193182;
  const i64 error_ns    = NANOSEC_IN_SEC / PIT_FREQ - ticktime_ns;

  const i64 error = error_ns * (i64)tick;

  timespec time = {
      .tv_sec  = (i64)tick / PIT_FREQ - error / NANOSEC_IN_SEC,
      .tv_nsec = (i64)(tick % PIT_FREQ) * (NANOSEC_IN_SEC / PIT_FREQ) - error % NANOSEC_IN_SEC,
  };
  if (time.tv_nsec >= NANOSEC_IN_SEC) {
    time.tv_sec++;
    time.tv_nsec -= NANOSEC_IN_SEC;
  }
  return time;
}

FASTCALL void inthandler20(i32 id, regs32 *regs) {
  system_tick++;

  kenel_debugger_tick();

  if (current_task->tid >= 0) task_tick();
}
