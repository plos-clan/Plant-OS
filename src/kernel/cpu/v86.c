#include <kernel.h>
typedef struct __PACKED__ {
  volatile u16    status; // 0 empty 1. ready 2. busy 3. done
  volatile u16    func;
  volatile regs16 regs;
} *v86_bios_request_t;

static volatile v86_bios_request_t v86_requests = null;
void                               entering_v86(u32 ss, u32 esp, u32 cs, u32 eip);
extern TSS32                       tss;
static u32                         v86_service_tid = 0;

void v86_task() {
  printf("v86_task\n");
  u32   pde       = current_task()->pde;
  void *code      = page_malloc_one();
  void *stack     = page_malloc_one();
  v86_requests    = page_malloc_one();
  v86_service_tid = current_task()->tid;
  vfs_node_t p    = vfs_open("/fatfs2/v86_service.bin");
  assert(p, "open /fatfs2/v86_service.bin failed");
  vfs_read(p, code, 0, p->size);
  page_link_addr_pde(0x2000, pde, (u32)code);
  memset(v86_requests, 0, PAGE_SIZE);
  page_link_addr_pde(0x3000, pde, (u32)v86_requests);

  page_link_addr_pde(0x1000, pde, (u32)stack);
  page_link_addr_pde(0x0, pde, 0x0);
  for (int i = 0x7000; i < 0x100000; i += 4096) {
    page_link_addr_pde(i, pde, i);
  }
  tss.esp0                  = current_task()->top;
  tss.eflags                = 0x202 | (1 << 17);
  tss.iomap                 = ((u32)offsetof(TSS32, io_map));
  current_task()->v86_mode  = 1;
  current_task()->user_mode = 1;
  current_task()->v86_if    = 1;
  entering_v86(0x100, 0xfff, 0x200, 0);
  for (;;)
    ;
}

void v86_int(u8 intnum, regs16 *regs) {
  while (!v86_requests)
    ;
  //   printf("v86_int: intnum=%d\n", intnum);
  while (v86_requests->status != 0)
    ;
  v86_requests->regs   = *regs;
  v86_requests->func   = intnum;
  v86_requests->status = 1;
  while (v86_requests->status != 3) {
    task_run(get_task(v86_service_tid));
    task_next();
  }
  *regs                = v86_requests->regs;
  v86_requests->status = 0;
}