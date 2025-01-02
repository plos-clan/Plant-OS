#include <kernel.h>

typedef struct __PACKED__ {
  i16    status; // 0 idle
                 // 1 request
                 // 2 running
                 // 3 done
  u16    func;
  regs16 regs;
} *v86_bios_request_t;

static volatile v86_bios_request_t v86_requests = null;

void entering_v86(u32 ss, u32 esp, u32 cs, u32 eip);

extern TSS32 tss;
static u32   v86_service_tid = 0;
task_t       v86_using_task  = null;

void v86_task_main() {
  klogd("v86_task is starting.");
  // 分配内存保存请求
  v86_bios_request_t ptr = page_malloc_one();
  assert(ptr != null, "page_malloc_one failed");
  atom_store(&ptr->status, 0); // 保证 0 一定被写入后执行
  v86_requests    = ptr;
  v86_service_tid = current_task->tid;
  // 读出代码
  void *code = page_malloc_one();
  assert(code != null, "page_malloc_one failed");
  vfs_node_t p = vfs_open("/fatfs2/v86_service.bin");
  assert(p, "open /fatfs2/v86_service.bin failed");
  vfs_read(p, code, 0, p->size);
  // 映射页面
  u32 pde = current_task->cr3;
  page_link_addr_pde(0x2000, pde, (usize)code);
  page_link_addr_pde(0x3000, pde, (usize)ptr);
  page_link_addr_pde(0x0, pde, 0x0);
  for (usize i = 0x7000; i < 0x100000; i += PAGE_SIZE) {
    page_link_addr_pde(i, pde, i);
  }
  // 设置状态
  tss.esp0                = current_task->stack_bottom;
  tss.eflags              = 0x202 | (1 << 17);
  tss.iomap               = offsetof(TSS32, io_map);
  current_task->v86_mode  = 1;
  current_task->user_mode = 1;
  current_task->v86_if    = 1;
  // 进入 V86
  entering_v86(0x300, 0x1000, 0x200, 0);
  infinite_loop;
}

void v86_int(byte intnum, regs16 *regs) {
  waituntil(v86_requests != null);
  val v86_task = task_by_id(v86_service_tid);

  for (i16 value = 0; !atom_cexch(&v86_requests->status, &value, 1); value = 0) {
    klogd("v86_int wait, status is %d", value);
    task_run(v86_task);
    task_next();
  }

  v86_using_task     = current_task;
  v86_requests->regs = *regs; // 写入数据
  v86_requests->func = intnum;

  atom_store(&v86_requests->status, 2); // 发送请求
  task_run(v86_task);
  task_next();

  while (atom_load(&v86_requests->status) != 3) { // 等待完成
    task_run(v86_task);
    task_next();
  }

  *regs          = v86_requests->regs; // 读出数据
  v86_using_task = null;

  atom_store(&v86_requests->status, 0); // 完成
}
