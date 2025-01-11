#include <kernel.h>

#pragma GCC optimize("O0")

void task_to_user_mode_elf();

extern TSS32     tss;
extern PageInfo *pages;

#define PIDX(addr) ((u32)(addr) >> 12)           // 获取 addr 的页索引
#define PDI(addr)  (((u32)(addr) >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define PTI(addr)  (((u32)(addr) >> 12) & 0x3ff) // 获取 addr 的页表索引

void task_app() {
  klogd("%s", current_task->command_line);
  klogd("%08x", current_task->stack_bottom);
  cir_queue8_t kfifo = malloc(sizeof(struct cir_queue8));
  cir_queue8_t mfifo = malloc(sizeof(struct cir_queue8));
  var          kbuf  = page_malloc_one();
  var          mbuf  = page_malloc_one();
  cir_queue8_init(kfifo, 4096, kbuf);
  cir_queue8_init(mfifo, 4096, mbuf);

  task_set_fifo(current_task, kfifo, mfifo);
  u32 pde = current_task->cr3;
  asm_cli;
  asm_set_cr3(PD_ADDRESS);
  current_task->cr3 = PD_ADDRESS;
  klogd("P1 %08x", current_task->cr3);
  for (int i = PDI(0x70000000) * 4; i < PAGE_SIZE; i += 4) {
    u32 *pde_entry = (u32 *)(pde + i);

    if ((*pde_entry & PAGE_SHARED) || pages[PIDX(*pde_entry)].count > 1) {
      if (pages[PIDX(*pde_entry)].count > 1) {
        u32 old    = *pde_entry & 0xfffff000;
        u32 attr   = *pde_entry & 0xfff;
        *pde_entry = (u32)page_malloc_one_count_from_4gb();
        memcpy((void *)(*pde_entry), (void *)old, PAGE_SIZE);
        pages[PIDX(old)].count--;
        *pde_entry |= PAGE_USER | PAGE_PRESENT | PAGE_WRABLE;
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
        *pte_entry |= PAGE_USER | PAGE_PRESENT;
      }
    }
  }
  current_task->cr3 = pde;
  asm_sti;
  asm_set_cr3(pde);
  klogd("go to task_to_usermode_elf");
  task_to_user_mode_elf();
  infinite_loop;
}

extern task_t mouse_use_task;

#define STACK_SIZE 1024 // 4MiB

void task_to_user_mode_elf() {
  page_link(TASK_ARGS_ADDR);

  struct args args = {.cmdline = current_task->command_line, .sp = (void *)TASK_ARGS_ADDR};
  parse_args(&args);
  used_val(*args.argv, *args.envp);
  klogd("argc: %d", args.argc);
  for (int i = 0; i < args.argc; i++) {
    klogd("argv[%d]: %s", i, args.argv[i]);
  }

  vfs_node_t file = vfs_open("/fatfs0/ld-plos.bin");
  if (file == null) {
    if (mouse_use_task == current_task) mouse_sleep(&mdec);
    kloge();
    task_exit(I32_MAX);
    __builtin_unreachable();
  }

  regs32 *iframe = (regs32 *)current_task->stack_bottom - 1;
  *iframe        = (regs32){};
  iframe->edi    = (size_t)args.argc;
  iframe->esi    = (size_t)args.argv;
  iframe->edx    = (size_t)args.envp;
  iframe->fs     = GET_SEL(RING3_DS, SA_RPL3);
  iframe->es     = GET_SEL(RING3_DS, SA_RPL3);
  iframe->ds     = GET_SEL(RING3_DS, SA_RPL3);
  iframe->cs     = GET_SEL(RING3_CS, SA_RPL3);
  iframe->flags  = (0 << 12 | 0b10 | 1 << 9);
  iframe->ss     = GET_SEL(RING3_DS, SA_RPL3);
  tss.eflags     = iframe->flags;

  klogd("%lu", file->size);
  char *elf_data = page_alloc(file->size);
  vfs_read(file, elf_data, 0, file->size);
  if (!elf32_is_validate((Elf32Header *)elf_data)) {
    page_free(elf_data, file->size);
    if (mouse_use_task == current_task) mouse_sleep(&mdec);
    kloge();
    task_exit(I32_MAX);
    __builtin_unreachable();
  }

  for (usize i = 1; i <= STACK_SIZE; i++) {
    page_link(0xf0000000 - i * PAGE_SIZE);
  }
  iframe->esp = 0xf0000000 - STACK_SIZE;
  iframe->eip = load_elf((Elf32Header *)elf_data);
  klogd("eip = %08x", iframe->eip);
  current_task->user_mode = 1;
  tss.esp0                = current_task->stack_bottom;

  asm volatile("mov %0, %%esp\n\t" ::"r"(iframe));
  asm volatile("jmp asm_inthandler_quit\n\t");
  __builtin_unreachable();
}

i32 os_execute(cstr filename, cstr line) {
  val backup = mouse_use_task;

  klogd("execute: %s by command %s", filename, line);

  task_t t = create_task(&task_app, 1, 1);

  val old                 = current_task->sigint_up;
  current_task->sigint_up = 0;
  t->sigint_up            = 1;

  val o                   = current_task->fifosleep;
  current_task->fifosleep = 1;

  t->command_line = page_alloc(strlen(line) + 1);
  strcpy(t->command_line, line);
  page_link_addr_pde(TASK_CMDLINE_ADDR, t->cr3, (u32)t->command_line);

  klogd("t->tid %d %d", t->tid, t->ptid);

  task_run(t);

  val status              = waittid(t->tid);
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
