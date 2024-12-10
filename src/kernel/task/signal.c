// This code is released under the MIT License

#include <kernel.h>

typedef struct signal_frame {
  u32 eip1;
  u32 edi;
  u32 esi;
  u32 ebp;
  // 虽然 pushad 把 esp 也压入，但 esp 是不断变化的，所以会被 popad 忽略
  u32 esp_dummy;

  u32 ebx;
  u32 edx;
  u32 ecx;
  u32 eax;

  u32 gs;
  u32 fs;
  u32 es;
  u32 ds;

  u32 eip;
} signal_frame_t;

// TODO: 给GUI接管
void signal_deal() {
  if (!current_task()) return;
  if (current_task()->signal_disable) { return; }
  task_t task;
  task = current_task();
  //klogd("B %d\n",task->signal);
  int sig = -1;
  if (task->signal & MASK32(SIGINT)) {
    sig           = 0;
    task->signal &= ~MASK32(SIGINT);
    if (task->handler[SIGINT]) {
      intr_frame_t   *i     = (intr_frame_t *)(task->top - sizeof(intr_frame_t));
      signal_frame_t *frame = (signal_frame_t *)(i->esp - sizeof(signal_frame_t));
      frame->edi            = i->edi;
      frame->esi            = i->esi;
      frame->ebp            = i->ebp;
      frame->esp_dummy      = i->esp_dummy;
      frame->ebx            = i->ebx;
      frame->ecx            = i->ecx;
      frame->edx            = i->edx;
      frame->eax            = i->eax;
      frame->gs             = i->gs;
      frame->fs             = i->fs;
      frame->es             = i->es;
      frame->ds             = i->ds;
      frame->eip            = i->eip;
      frame->eip1           = task->ret_to_app;
      i->eip                = task->handler[SIGINT];
      i->esp                = (u32)frame;
      return;
    } else {

      // task_exit(0);
    }
  } else if (task->signal & MASK32(SIGKIL)) {
    task_exit(0);
  } else {
    return;
  }
}

void set_signal_handler(u32 sig, u32 handler) {
  current_task()->handler[sig] = handler;
}
