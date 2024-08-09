#include <kernel.h>
#pragma GCC optimize("O0")
void        task_to_user_mode_elf(char *filename);

extern char             *shell_data;
extern struct TSS32      tss;
extern struct PAGE_INFO *pages;

void task_to_user_mode_shell();

#define IDX(addr)  ((unsigned)addr >> 12)           // 获取 addr 的页索引
#define DIDX(addr) (((unsigned)addr >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define TIDX(addr) (((unsigned)addr >> 12) & 0x3ff) // 获取 addr 的页表索引
#define PAGE(idx)  ((unsigned)idx << 12) // 获取页索引 idx 对应的页开始的位置

static u32 padding_up(u32 num, u32 size) {
  return (num + size - 1) / size;
}

void task_app() {
  klogd("task_app");
  char *filename;
  while (!current_task()->line) {
    asm_sti;
    task_next();
    klogd("%d", current_task()->line);
  }
  klogd("1");
  unsigned *r          = current_task()->line;
  filename             = (char *)r[0];
  current_task()->line = (char *)r[1];
  klogd("我爱你");
  page_free_one(r);
  klogd("%08x", current_task()->top);
  klogd("%p %d", current_task()->nfs, vfs_filesize("testapp.bin"));
  char *kfifo = (char *)page_malloc_one();
  char *mfifo = (char *)page_malloc_one();
  char *kbuf  = (char *)page_malloc_one();
  char *mbuf  = (char *)page_malloc_one();

  cir_queue_init((cir_queue_t)kfifo, 4096, (u8 *)kbuf);
  cir_queue_init((struct FIFO8 *)mfifo, 4096, (u8 *)mbuf);
  task_set_fifo(current_task(), (struct FIFO8 *)kfifo, (struct FIFO8 *)mfifo);
  current_task()->alloc_size    = (u32 *)malloc(4);
  current_task()->alloced       = 1;
  *(current_task()->alloc_size) = 2 * 1024 * 1024;
  unsigned pde                  = current_task()->pde;
  asm_cli;
  asm_set_cr3(PDE_ADDRESS);
  klogd("P1 %08x", current_task()->pde);
  for (int i = DIDX(0x70000000) * 4; i < 0x1000; i += 4) {
    u32 *pde_entry = (u32 *)(pde + i);

    if ((*pde_entry & PAGE_SHARED) || pages[IDX(*pde_entry)].count > 1) {
      // if (pde_entry == 0x08e6b718) {
      //   while (true)
      //     ;
      // }
      if (pages[IDX(*pde_entry)].count > 1) {
        u32 old    = *pde_entry & 0xfffff000;
        u32 attr   = *pde_entry & 0xfff;
        *pde_entry = (unsigned)page_malloc_one_count_from_4gb();
        memcpy((void *)(*pde_entry), (void *)old, 0x1000);
        pages[IDX(old)].count--;
        *pde_entry |= PAGE_USER | PAGE_P | PAGE_WRABLE;
      } else {
        *pde_entry &= 0xfffff;
        *pde_entry |= 7;
      }
    }
    unsigned p = *pde_entry & (0xfffff000);
    for (int j = 0; j < 0x1000; j += 4) {
      u32 *pte_entry = (u32 *)(p + j);
      if ((*pte_entry & PAGE_SHARED)) {
        *pte_entry &= 0xfffff000;
        *pte_entry |= PAGE_USER | PAGE_P;
      }
    }
  }
  asm_sti;
  asm_set_cr3(pde);
  char tmp[100];
  task_to_user_mode_elf(filename);
  while (true) {}
}
void task_shell() {
  while (!current_task()->line) {}
  char *kfifo = (char *)page_malloc_one();
  char *mfifo = (char *)page_malloc_one();
  char *kbuf  = (char *)page_malloc_one();
  char *mbuf  = (char *)page_malloc_one();
  cir_queue_init((struct FIFO8 *)kfifo, 4096, (u8 *)kbuf);
  cir_queue_init((struct FIFO8 *)mfifo, 4096, (u8 *)mbuf);
  task_set_fifo(current_task(), (cir_queue_t)kfifo, (cir_queue_t)mfifo);
  current_task()->alloc_size    = (u32 *)malloc(4);
  current_task()->alloced       = 1;
  *(current_task()->alloc_size) = 1 * 1024 * 1024;

  unsigned pde = current_task()->pde;
  asm_cli;
  asm_set_cr3(PDE_ADDRESS);
  for (int i = DIDX(0x70000000) * 4; i < 0x1000; i += 4) {
    u32 *pde_entry = (u32 *)(pde + i);
    if ((*pde_entry & PAGE_SHARED) || pages[IDX(*pde_entry)].count > 1) {

      if (pages[IDX(*pde_entry)].count > 1) {
        u32 old    = *pde_entry & 0xfffff000;
        u32 attr   = *pde_entry & 0xfff;
        *pde_entry = (unsigned)page_malloc_one_count_from_4gb();
        memcpy((void *)(*pde_entry), (void *)old, 0x1000);
        pages[IDX(old)].count--;
        *pde_entry |= PAGE_USER | PAGE_P | PAGE_WRABLE;
      } else {
        *pde_entry &= 0xfffff;
        *pde_entry |= 7;
      }
    } else {
    }
    unsigned p = *pde_entry & (0xfffff000);
    for (int j = 0; j < 0x1000; j += 4) {
      u32 *pte_entry = (u32 *)(p + j);
      if ((*pte_entry & PAGE_SHARED)) {
        *pte_entry &= 0xfffff000;
        *pte_entry |= PAGE_USER | PAGE_P;
      }
    }
  }
  asm_sti;
  asm_set_cr3(pde);
  char tmp[100];
  task_to_user_mode_shell();
  while (true)
    ;
}
void task_to_user_mode_shell() {
  unsigned addr = (unsigned)current_task()->top;

  addr                 -= sizeof(intr_frame_t);
  intr_frame_t *iframe  = (intr_frame_t *)(addr);
  iframe->edi           = 0;          // argc
  iframe->esi           = 0xf0000001; // argv
  iframe->ebp           = 3;
  iframe->esp_dummy     = 4;
  iframe->ebx           = 5;
  iframe->edx           = NULL; // envp没有
  iframe->ecx           = 7;
  iframe->eax           = 8;

  iframe->gs     = GET_SEL(5 * 8, SA_RPL3);
  iframe->ds     = GET_SEL(3 * 8, SA_RPL3);
  iframe->es     = GET_SEL(3 * 8, SA_RPL3);
  iframe->fs     = GET_SEL(3 * 8, SA_RPL3);
  iframe->ss     = GET_SEL(3 * 8, SA_RPL3);
  iframe->cs     = GET_SEL(4 * 8, SA_RPL3);
  iframe->eflags = (0 << 12 | 0b10 | 1 << 9);
  iframe->esp    = NULL; // 设置用户态堆栈
  char *p        = shell_data;
  if (!elf32Validate(p)) {
    extern mtask *mouse_use_task;
    if (mouse_use_task == current_task()) { mouse_sleep(&mdec); }
    list_free_with(vfs_now->path, free);
    free(vfs_now->cache);
    free((void *)vfs_now);
    task_exit(-1);
    while (true)
      ;
  }
  unsigned alloc_addr = (elf32_get_max_vaddr(p) & 0xfffff000) + 0x1000;
  unsigned pg         = padding_up(*(current_task()->alloc_size), 0x1000);
  for (int i = 0; i < pg + 128; i++) {
    // klog("%d\n",i);
    page_link(alloc_addr + i * 0x1000);
  }
  unsigned alloced_esp  = alloc_addr + 128 * 0x1000;
  alloc_addr           += 128 * 0x1000;
  iframe->esp           = alloced_esp;
  page_link(0xf0000000);
  *(u8 *)(0xf0000000)        = 1;
  current_task()->alloc_addr = alloc_addr;

  iframe->eip               = load_elf(p);
  current_task()->user_mode = 1;
  tss.esp0                  = current_task()->top;
  change_page_task_id(current_task()->tid, (void *)(iframe->esp - 512 * 1024), 512 * 1024);

  asm volatile("movl %0, %%esp\n\t"
               "xchg %%bx,%%bx\n\t"
               "popa\n\t"
               "pop %%gs\n\t"
               "pop %%fs\n\t"
               "pop %%es\n\t"
               "pop %%ds\n\t"
               "iret" ::"m"(iframe));
  while (true) {}
}

void task_to_user_mode_elf(char *filename) {
  page_link(0xf0000000); // 配置空间
  unsigned addr = (unsigned)current_task()->top;
  // 配置空间放置在0xf0000000 用于记录一些启动信息

  struct args args = {
      .path    = filename,
      .cmdline = current_task()->line,
      .sp      = (void *)0xf0000001,
  };
  parse_args(&args);
  klogd("argc: %d", args.argc);
  for (int i = 0; i < args.argc; i++) {
    klogd("argv[%d]: %s", i, args.argv[i]);
  }

  addr                 -= sizeof(intr_frame_t);
  intr_frame_t *iframe  = (intr_frame_t *)(addr);
  // 这里可以改变跳转后各个寄存器的初始值
  iframe->edi       = (size_t)args.argc; // argc
  iframe->esi       = (size_t)args.argv; // argv
  iframe->ebp       = 3;
  iframe->esp_dummy = 4;
  iframe->ebx       = 5;
  iframe->edx       = (size_t)args.envp; // envp
  iframe->ecx       = 7;
  iframe->eax       = 8;

  iframe->gs     = GET_SEL(5 * 8, SA_RPL3);
  iframe->ds     = GET_SEL(3 * 8, SA_RPL3);
  iframe->es     = GET_SEL(3 * 8, SA_RPL3);
  iframe->fs     = GET_SEL(3 * 8, SA_RPL3);
  iframe->ss     = GET_SEL(3 * 8, SA_RPL3);
  iframe->cs     = GET_SEL(4 * 8, SA_RPL3);
  iframe->eflags = (0 << 12 | 0b10 | 1 << 9);
  iframe->esp    = NULL; // 设置用户态堆栈
  tss.eflags     = 0x202;
  u32 sz         = vfs_filesize(filename);
  klogd("%d", sz);
  char *p = page_malloc(sz);
  vfs_readfile(filename, p);
  klogd();
  if (!elf32Validate(p)) {
    klogd();
    for (int i = 0; i < 0x200; i++) {
      // printf("%02x ", p[i]);
    }
    for (;;)
      ;
    extern mtask *mouse_use_task;
    if (mouse_use_task == current_task()) { mouse_sleep(&mdec); }
    page_free(p, vfs_filesize(filename));
    list_free_with(vfs_now->path, free);
    free(vfs_now->cache);
    free((void *)vfs_now);
    task_exit(-1);
    while (true)
      ;
  }
  unsigned alloc_addr = (elf32_get_max_vaddr(p) & 0xfffff000) + 0x1000;
  unsigned pg         = padding_up(*(current_task()->alloc_size), 0x1000);
  for (int i = 0; i < pg + 128 * 4; i++) {
    page_link(alloc_addr + i * 0x1000);
  }
  unsigned alloced_esp  = alloc_addr + 128 * 0x1000 * 4;
  alloc_addr           += 128 * 0x1000 * 4;
  iframe->esp           = alloced_esp;
  if (current_task()->ptid != -1) { *(u8 *)(0xf0000000) = 0; }
  //  strcpy((char *)0xf0000001, current_task()->line);
  // *(u32 *)(0xb5000000) = 2;
  // klog("value = %08x\n",*(u32 *)(0xb5000000));
  current_task()->alloc_addr = alloc_addr;
  iframe->eip                = load_elf(p);
  klog("eip = %08x\n", &(iframe->eip));
  current_task()->user_mode = 1;
  tss.esp0                  = current_task()->top;
  change_page_task_id(current_task()->tid, p, vfs_filesize(filename));
  change_page_task_id(current_task()->tid, (void *)(iframe->esp - 512 * 1024), 512 * 1024);
  //klog("%d\n", get_interrupt_state());
  asm volatile("movl %0, %%esp\n"
               "popa\n"
               "pop %%gs\n"
               "pop %%fs\n"
               "pop %%es\n"
               "pop %%ds\n"
               "iret" ::"m"(iframe));
  while (true) {}
}

int os_execute(char *filename, char *line) {
  extern mtask *mouse_use_task;
  mtask        *backup = mouse_use_task;
  extern int    init_ok_flag;
  char         *fm = (char *)malloc(strlen(filename) + 1);
  strcpy(fm, filename);
  init_ok_flag = 0;

  klogd("execute: %s %s", filename, line);

  mtask *t = create_task((u32)task_app, 0, 1, 1);
  // 轮询
  t->train = 0;
  vfs_change_disk_for_task(current_task()->nfs->drive, t);

  char *path;
  //   list_foreach(current_task()->nfs->path, l) {
  //     path = (char *)l->data;
  //     t->nfs->cd(t->nfs, path);
  //   }
  init_ok_flag              = 1;
  t->ptid                   = current_task()->tid;
  int old                   = current_task()->sigint_up;
  current_task()->sigint_up = 0;
  t->sigint_up              = 1;
  struct tty *tty_backup    = current_task()->TTY;
  t->TTY                    = current_task()->TTY;

  current_task()->TTY = NULL;
  char *p1            = malloc(strlen(line) + 1);
  klogi("%d", t->nfs->FileSize(t->nfs, "TESTAPP.BIN"));
  strcpy(p1, line);
  int o                     = current_task()->fifosleep;
  current_task()->fifosleep = 1;
  unsigned *r               = page_malloc_one_no_mark();
  r[0]                      = (u32)fm;
  r[1]                      = (u32)p1;
  t->line                   = r;

  klogd();

  unsigned status           = waittid(t->tid);
  current_task()->fifosleep = o;

  klogd();

  free(p1);
  free(fm);
  current_task()->TTY = tty_backup;
  if (backup) {
    mouse_ready(&mdec);
    mouse_use_task = backup;
  } else {
    mouse_sleep(&mdec);
  }
  current_task()->sigint_up = old;

  klogd();

  return status;
}

int os_execute_shell(char *line) {
  extern int init_ok_flag;
  init_ok_flag              = 0;
  mtask *t                  = create_task((u32)task_shell, 0, 1, 1);
  t->nfs                    = current_task()->nfs;
  t->train                  = 1;
  int old                   = current_task()->sigint_up;
  current_task()->sigint_up = 0;
  t->sigint_up              = 1;
  init_ok_flag              = 1;
  t->ptid                   = current_task()->tid;
  struct tty *tty_backup    = current_task()->TTY;
  t->TTY                    = current_task()->TTY;
  current_task()->TTY       = NULL;
  char *p1                  = malloc(strlen(line) + 1);
  strcpy(p1, line);
  int o                     = current_task()->fifosleep;
  current_task()->fifosleep = 1;
  t->line                   = p1;
  // io_sti();
  unsigned status           = waittid(t->tid);
  current_task()->fifosleep = o;
  free(p1);
  current_task()->TTY       = tty_backup;
  current_task()->sigint_up = old;
  return status;
}

void os_execute_no_ret(char *filename, char *line) {
  mtask      *t          = create_task((u32)task_app, 0, 1, 1);
  struct tty *tty_backup = current_task()->TTY;
  t->TTY                 = current_task()->TTY;
  current_task()->TTY    = NULL;
  unsigned *r            = page_malloc_one_no_mark();
  r[0]                   = (u32)filename;
  r[1]                   = (u32)line;
  t->line                = r;
}
