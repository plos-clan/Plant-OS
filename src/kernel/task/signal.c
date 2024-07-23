#include <kernel.h>
typedef struct intr_frame1_t {
  unsigned eip1;
  unsigned edi;
  unsigned esi;
  unsigned ebp;
  // 虽然 pushad 把 esp 也压入，但 esp 是不断变化的，所以会被 popad 忽略
  unsigned esp_dummy;

  unsigned ebx;
  unsigned edx;
  unsigned ecx;
  unsigned eax;

  unsigned gs;
  unsigned fs;
  unsigned es;
  unsigned ds;

  unsigned eip;
} signal_frame_t;
void b2() {

  while (true)
    ;
  return;
}
// TODO: 给GUI接管
void signal_deal() {
  if (!current_task()) return;
  if (current_task()->signal_disable) { return; }
  mtask *task;
  task = current_task();
  //logk("B %d\n",task->signal);
  int sig = -1;
  if (task->signal & SIGMASK(SIGINT)) {
    sig           = 0;
    task->signal &= ~SIGMASK(SIGINT);
    if (task->handler[SIGINT]) {
      intr_frame_t   *i     = (void *)(task->top - sizeof(intr_frame_t));
      signal_frame_t *frame = (unsigned *)(i->esp - sizeof(signal_frame_t));
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
  } else if (task->signal & SIGMASK(SIGKIL)) {
    task_exit(0);
  } else {
    return;
  }
}
void set_signal_handler(unsigned sig, unsigned handler) {
  current_task()->handler[sig] = handler;
}