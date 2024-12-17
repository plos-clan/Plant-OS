#include <kernel.h>

#pragma GCC optimize("O0")

void task_to_user_mode_elf();

extern TSS32     tss;
extern PageInfo *pages;

#define IDX(addr)  ((u32)(addr) >> 12)           // 获取 addr 的页索引
#define DIDX(addr) (((u32)(addr) >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define TIDX(addr) (((u32)(addr) >> 12) & 0x3ff) // 获取 addr 的页表索引
#define PAGE(idx)  ((u32)(idx) << 12)            // 获取页索引 idx 对应的页开始的位置

void task_app() {
  klogd("task_app");
  while (!current_task->command_line) {
    task_next();
  }
  klogd("%s", current_task->command_line);
  klogd("%08x", current_task->stack_bottom);
  // klogd("%p %d", current_task->nfs, vfs_filesize("testapp.bin"));
  char *kfifo = (char *)page_malloc_one();
  char *mfifo = (char *)page_malloc_one();
  char *kbuf  = (char *)page_malloc_one();
  char *mbuf  = (char *)page_malloc_one();

  cir_queue8_init((cir_queue8_t)kfifo, 4096, (u8 *)kbuf);
  cir_queue8_init((cir_queue8_t)mfifo, 4096, (u8 *)mbuf);
  task_set_fifo(current_task, (cir_queue8_t)kfifo, (cir_queue8_t)mfifo);
  current_task->alloc_size    = (u32 *)malloc(4);
  current_task->alloced       = 1;
  *(current_task->alloc_size) = 2 * 1024 * 1024;
  u32 pde                     = current_task->pde;
  asm_cli;
  asm_set_cr3(PDE_ADDRESS);
  current_task->pde = PDE_ADDRESS;
  klogd("P1 %08x", current_task->pde);
  for (int i = DIDX(0x70000000) * 4; i < PAGE_SIZE; i += 4) {
    u32 *pde_entry = (u32 *)(pde + i);

    if ((*pde_entry & PAGE_SHARED) || pages[IDX(*pde_entry)].count > 1) {
      // if (pde_entry == 0x08e6b718) {
      //   while (true)
      //     ;
      // }
      if (pages[IDX(*pde_entry)].count > 1) {
        u32 old    = *pde_entry & 0xfffff000;
        u32 attr   = *pde_entry & 0xfff;
        *pde_entry = (u32)page_malloc_one_count_from_4gb();
        memcpy((void *)(*pde_entry), (void *)old, PAGE_SIZE);
        pages[IDX(old)].count--;
        *pde_entry |= PAGE_USER | PAGE_P | PAGE_WRABLE;
      } else {
        *pde_entry &= 0xfffff;
        *pde_entry |= 7;
      }
    }
    u32 p = *pde_entry & (0xfffff000);
    for (int j = 0; j < PAGE_SIZE; j += 4) {
      u32 *pte_entry = (u32 *)(p + j);
      if ((*pte_entry & PAGE_SHARED)) {
        *pte_entry &= 0xfffff000;
        *pte_entry |= PAGE_USER | PAGE_P;
      }
    }
  }
  current_task->pde = pde;
  asm_sti;
  asm_set_cr3(pde);
  klogd("go to task_to_usermode_elf");
  task_to_user_mode_elf();
  infinite_loop;
}

#define STACK_SIZE 1024 // 4MiB

void task_to_user_mode_elf() {
  page_link(0xf0000000);

  struct args args = {.cmdline = current_task->command_line, .sp = (void *)0xf0000000};
  parse_args(&args);
  klogd("argc: %d", args.argc);
  for (int i = 0; i < args.argc; i++) {
    klogd("argv[%d]: %s", i, args.argv[i]);
  }

  vfs_node_t file = vfs_open(args.argv[0]);
  if (!file) goto err;

  regs32 *iframe = (regs32 *)current_task->stack_bottom - 1;
  iframe->edi    = (size_t)args.argc;
  iframe->esi    = (size_t)args.argv;
  iframe->ebp    = 0;
  iframe->_      = 0;
  iframe->ebx    = 0;
  iframe->edx    = (size_t)args.envp;
  iframe->ecx    = 0;
  iframe->eax    = 0;
  iframe->gs     = GET_SEL(5 * 8, SA_RPL3);
  iframe->fs     = GET_SEL(3 * 8, SA_RPL3);
  iframe->es     = GET_SEL(3 * 8, SA_RPL3);
  iframe->ds     = GET_SEL(3 * 8, SA_RPL3);
  iframe->id     = 0;
  iframe->err    = 0;
  iframe->eip    = 0;
  iframe->cs     = GET_SEL(4 * 8, SA_RPL3);
  iframe->flags  = (0 << 12 | 0b10 | 1 << 9);
  iframe->esp    = 0;
  iframe->ss     = GET_SEL(3 * 8, SA_RPL3);
  tss.eflags     = 0x202;

  klogd("%lu", file->size);
  char *p = page_alloc(file->size);
  vfs_read(file, p, 0, file->size);
  klogd();
  if (!elf32_is_validate((Elf32_Ehdr *)p)) {
    klogd();
    extern task_t mouse_use_task;
    page_free(p, file->size);
  err:

    if (mouse_use_task == current_task) mouse_sleep(&mdec);

    kloge();
    task_exit(I32_MAX);
    __builtin_unreachable();
  }
  u32 alloc_addr = (elf32_get_max_vaddr((Elf32_Ehdr *)p) & 0xfffff000) + PAGE_SIZE;
  klogd("alloc_addr = %08x", alloc_addr);

  u32 pg = PADDING_UP(*(current_task->alloc_size), PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < pg + STACK_SIZE; i++) {
    page_link(alloc_addr + i * PAGE_SIZE);
  }
  u32 alloced_esp           = alloc_addr + STACK_SIZE * PAGE_SIZE;
  alloc_addr               += STACK_SIZE * PAGE_SIZE;
  iframe->esp               = alloced_esp;
  current_task->alloc_addr  = alloc_addr;
  current_task->v86_mode    = 0;
  iframe->eip               = load_elf((Elf32_Ehdr *)p);
  klogd("eip = %08x", iframe->eip);
  current_task->user_mode = 1;
  tss.esp0                = current_task->stack_bottom;
  change_page_task_id(current_task->tid, p, file->size);

  asm volatile("mov %0, %%esp\n\t" ::"r"(iframe));
  asm volatile("jmp asm_inthandler_quit\n\t");
  __builtin_unreachable();
}

extern task_t mouse_use_task;

i32 os_execute(cstr filename, cstr line) {
  task_t backup = mouse_use_task;

  klogd("execute: %s by command %s", filename, line);

  task_t t = create_task(&task_app, 1, 1);

  const int old           = current_task->sigint_up;
  current_task->sigint_up = 0;
  t->sigint_up            = 1;

  const int o             = current_task->fifosleep;
  current_task->fifosleep = 1;

  t->command_line = strdup(line);

  klogd("t->tid %d %d", t->tid, t->ptid);

  i32 status              = waittid(t->tid);
  current_task->fifosleep = o;

  klogd();

  if (backup) {
    mouse_ready(&mdec);
    mouse_use_task = backup;
  } else {
    mouse_sleep(&mdec);
  }
  current_task->sigint_up = old;

  klogd();

  return status;
}
