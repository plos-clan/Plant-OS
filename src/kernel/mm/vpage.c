#include <kernel.h>

#define IDX(addr) ((u32)(addr) >> 12)           // 获取 addr 的页索引
#define PDI(addr) (((u32)(addr) >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define PTI(addr) (((u32)(addr) >> 12) & 0x3ff) // 获取 addr 的页表索引
#define PAGE(idx) ((u32)(idx) << 12)            // 获取页索引 idx 对应的页开始的位置

//* ----------------------------------------------------------------------------------------------------
//; 页表管理
//* ----------------------------------------------------------------------------------------------------

static inthandler_f page_fault;

// 刷新虚拟地址 vaddr 的 块表 TLB
finline void flush_tlb(usize vaddr) {
  asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

#define flush_tlb(vaddr) flush_tlb((usize)(vaddr))

PageInfo *pages = (PageInfo *)PAGE_MANNAGER;

static void init_pdepte(size_t *pd, size_t *pt, size_t *page_end) {
  for (size_t i = 0; pd + i < pt; i++) {
    pd[i] = ((size_t)pt + i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRABLE;
  }
  for (size_t i = 0; pt + i < page_end; i++) {
    pt[i] = (i * PAGE_SIZE) | PAGE_PRESENT | PAGE_WRABLE;
  }
}

// TODO 重构啊
void page_manager_init(PageInfo *pg) { // 全部置为0就好
  memset(pg, 0, 1024 * 1024 * sizeof(PageInfo));
}

void page_set_alloced(PageInfo *pg, u32 start, u32 end) {
  for (int i = IDX(start); i <= IDX(end); i++) {
    pg[i].count++; // 设置占用，但是没有进程引用
  }
}

// kernel 加载到0x280000
// 保留 0 到 0xb00000 (不含)
// 0xc0000000 到 0xffffffff 物理内存保留地址

// 0xf0000000 以后可能是硬件使用的内存

// 某些设计思路：
// 从0x70000000开始，到0xf0000000
// 大概2GB的内存可以给应用程序分配，OS使用前0x70000000的内存地址
// 为了防止应用程序和操作系统抢占前0x70000000的内存，所以page_link和copy_on_write是从后往前分配的
// OS应该是用不完0x70000000的，所以应用程序大概是可以用满2GB

usize pd_clone(usize addr) {
  const var pd = (PDE *)addr;
  for (usize i = PDI(0x70000000); i < 1024; i++) {
    var pde = &pd[i];
    pages[pde->addr].count++;
    pde->wrable = false;
    var pt      = (PTE *)(pde->addr << 12);
    for (usize j = 0; j < 1024; j++) {
      var pte = &pt[j];
      if (pte->user) pages[pte->addr].count++;
      pte->wrable = pte->shared ? true : false;
    }
  }
  var result = page_malloc_one();
  memcpy(result, (void *)addr, PAGE_SIZE);
  flush_tlb(result);
  flush_tlb(addr);
  asm_set_cr3(addr);
  return (usize)result;
}

static void pde_reset(u32 addr) {
  for (int i = PDI(0x70000000) * 4; i < PAGE_SIZE; i += 4) {
    u32 *pde  = (u32 *)(addr + i);
    *pde     |= PAGE_WRABLE;
  }
}

void pd_free(usize addr) {
  if (addr == PD_ADDRESS) return;
  for (int i = PDI(0x70000000) * 4; i < PDI(0xf1000000) * 4; i += 4) {
    u32 *pde_entry = (u32 *)(addr + i);
    u32  p         = *pde_entry & (0xfffff000);
    if (!(*pde_entry & PAGE_USER) && !(*pde_entry & PAGE_PRESENT)) { continue; }
    for (int j = 0; j < PAGE_SIZE; j += 4) {
      u32 *pte_entry = (u32 *)(p + j);
      if (*pte_entry & PAGE_USER && *pte_entry & PAGE_PRESENT) { pages[IDX(*pte_entry)].count--; }
    }

    pages[IDX(*pde_entry)].count--;
  }
  flush_tlb(addr);
  page_free((void *)addr, PAGE_SIZE);
}

static void page_link_pde(u32 addr, u32 pd) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(addr);
  p        = (addr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pd + t * 4));

  if (pages[IDX(*pte)].count > 1) {
    // 这个页目录还有人引用，所以需要复制
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4); // PTE页表
  // COW
  if (pages[IDX(*physics)].count > 1) { pages[IDX(*physics)].count--; }
  *physics  = (u32)page_malloc_one_count_from_4gb();
  *physics |= 7;
  flush_tlb((u32)pte);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

void page_link_addr_pde(usize addr, usize pde, usize map_addr) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(addr);
  p        = (addr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde + t * 4));

  if (pages[IDX(*pte)].count > 1) {
    // 这个页目录还有人引用，所以需要复制
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4); // PTE页表
  // COW
  if (pages[IDX(*physics)].count > 1) { pages[IDX(*physics)].count--; }
  *physics  = (u32)map_addr;
  *physics |= 7;
  flush_tlb((u32)pte);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

static void page_link_pde_share(u32 addr, u32 pde) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(addr);
  p        = (addr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde + t * 4));

  if (pages[IDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) {
    // 这个页目录还有人引用，所以需要复制
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4); // PTE页表
  // COW
  if (pages[IDX(*physics)].count > 1) { pages[IDX(*physics)].count--; }
  int flag = 0;
  if (*physics & PAGE_SHARED) {
    klogd("THIS\n");
    flag = 1;
  }
  *physics  = (u32)page_malloc_one_count_from_4gb();
  *physics |= 7;
  if (flag) { *physics |= PAGE_SHARED; }
  flush_tlb((u32)pte);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

static void page_link_pde_paddr(u32 addr, u32 pde, u32 *paddr1, u32 paddr2) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(addr);
  p        = (addr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde + t * 4));
  // klogd("*pte = %08x\n",*pte);
  if (pages[IDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) {
    int flag;
    if (*pte & PAGE_SHARED) flag = 1;
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = *paddr1;
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte    |= 7;
    *paddr1  = 0;
    if (flag) { *pte |= PAGE_SHARED; }
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4);
  if (pages[IDX(*physics)].count > 1) { pages[IDX(*physics)].count--; }
  int flag = 0;
  if (*physics & PAGE_SHARED) { flag = 1; }
  *physics  = paddr2;
  *physics |= 7;
  if (flag) { *physics |= PAGE_SHARED; }
  flush_tlb((u32)pte);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

u32 page_get_alloced() {
  u32 r = 0;
  for (int i = 0; i < PADDING_UP(total_mem_size, PAGE_SIZE) / PAGE_SIZE; i++) {
    if (pages[i].count) r++;
  }
  return r;
}

void page_links_pde(u32 start, u32 numbers, u32 pde) {
  int i     = 0;
  int times = 0;
  u32 a[2];
  int j = 0;
  for (i = IDX(total_mem_size) - 1; i >= 0 && times < numbers; i--) {
    if (pages[i].count == 0) {
      u32 addr = PAGE(i);
      pages[i].count++;
      a[j++] = addr;
    }
    if (j == 2) {
      page_link_pde_paddr(start, pde, &(a[0]), a[1]);
      times++;
      start += PAGE_SIZE;
      j      = 0;
      if (a[0] != 0) { j = 1; }
    }
  }
  if (j) { page_free((void *)a[j - 1], PAGE_SIZE); }
}

void page_links(u32 start, u32 numbers) {
  page_links_pde(start, numbers, current_task->cr3);
}

void page_link(u32 addr) {
  page_link_pde(addr, current_task->cr3);
}

void page_link_share(u32 addr) {
  page_link_pde_share(addr, current_task->cr3);
}

void copy_from_phy_to_line(u32 phy, u32 line, u32 pde, u32 size) {
  size_t pg = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < pg; i++) {
    memcpy((void *)page_get_phy(pde, line), (void *)phy, min(size, PAGE_SIZE));
    size -= PAGE_SIZE;
    line += PAGE_SIZE;
    phy  += PAGE_SIZE;
  }
}

void set_line_address(u32 val, u32 line, u32 pde, u32 size) {
  size_t pg = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < pg; i++) {
    memset((void *)page_get_phy(pde, line), val, min(size, PAGE_SIZE));
    size -= PAGE_SIZE;
    line += PAGE_SIZE;
  }
}

void page_unlink_pde(u32 addr, u32 pde) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(addr);
  p        = (addr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde + t * 4));

  if (pages[IDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) {
    // 这个页目录还有人引用，所以需要复制
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4); // PTE页表
  // COW
  if (pages[IDX(*physics)].count > 1) { pages[IDX(*physics)].count--; }
  *physics = NULL;
  flush_tlb((u32)pte);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

void page_unlink(u32 addr) {
  page_unlink_pde(addr, current_task->cr3);
}

void init_paging() {
  inthandler_set(0x0e, &page_fault);
  init_pdepte((void *)PD_ADDRESS, (void *)PT_ADDRESS, (void *)PAGE_END);
  page_manager_init(pages);
  // kernel 加载到0x280000
  // 保留 0 到 0xb00000 (不含)
  page_set_alloced(pages, 0, 0xb00000);
  // 0xc0000000 到 0xffffffff 物理内存保留地址
  page_set_alloced(pages, 0xc0000000, 0xffffffff);
  asm_set_cr3(PD_ADDRESS);
  asm_set_pg;
}

void pf_set(u32 memsize) {
  u32 *pte = (u32 *)PT_ADDRESS;
  for (int i = 0; pte != (u32 *)PAGE_END; pte++, i++) {
    if (i >= memsize / 4096 && i <= 0xc0000000 / 4096) { *pte = 0; }
  }
}

int get_page_from_line_address(int line_address) {
  int t, p, page;
  t = line_address >> 22;
  p = (line_address >> 12) & 0x3ff;
  tpo2page(&page, t, p);
  return page;
}

void page2tpo(int page, int *t, int *p) {
  *t = page / 1024;
  *p = page % 1024;
}

void tpo2page(int *page, int t, int p) {
  *page = (t * 1024) + p;
}

void *page_malloc_one() {
  for (int i = 0; i != 1024 * 1024; i++) {
    if (pages[i].count == 0) {
      int t, p;
      page2tpo(i, &t, &p);
      u32 addr = mk_linear_addr(t, p, 0);
      pages[i].count++;
      return (void *)addr;
    }
  }
  return NULL;
}

void *page_malloc_one_count_from_4gb() {
  for (int i = IDX(total_mem_size) - 1; i >= 0; i--) {
    if (pages[i].count == 0) {
      u32 addr = PAGE(i);
      pages[i].count++;
      return (void *)addr;
    }
  }
  return NULL;
}

int get_pageinpte_address(int t, int p) {
  int page;
  tpo2page(&page, t, p);
  return (PT_ADDRESS + page * 4);
}

int find_kpage(int line, int n) {
  int free = 0;
  // 找一个连续的线性地址空间
  for (; line != 1024 * 1024; line++) {
    if (pages[line].count == 0) {
      free++;
    } else {
      free = 0;
    }
    if (free == n) {
      for (int j = line - n + 1; j != line + 1; j++) {
        pages[j].count++;
      }
      line -= n - 1;
      break;
    }
  }
  return line;
}

void *page_alloc(size_t size) {
  int n = ((size - 1) / (4 * 1024)) + 1;
  int i = find_kpage(0, n);
  int t, p;
  page2tpo(i, &t, &p);
  bzero((char *)mk_linear_addr(t, p, 0), n * 4 * 1024);
  u32 addr = mk_linear_addr(t, p, 0);
  return (void *)addr;
}

void page_free(void *p, size_t size) {
  if (((size_t)p & (PAGE_SIZE - 1)) != 0) fatal("炸啦！");
  for (size_t id = (size_t)p / PAGE_SIZE, i = 0; i < size; i += PAGE_SIZE, id++) {
    if (id >= 1024 * 1024) fatal("炸啦！"); // 超过最大页
    // page_free只应用于free page_malloc分配的内存，因此count直接设置为0
    // 如果是page_link出来的，那么task_free_all_pages会清理，free_pde也会清理
    pages[id].count = 0;
  }
}

void *phy_addr_from_linear_addr(void *line) {
  int t, p;
  page2tpo(get_page_from_line_address((int)line), &t, &p);
  return (void *)(*(int *)get_pageinpte_address(t, p));
}

void set_phy_addr_of_linear_addr(void *line, void *phy) {
  int t, p;
  page2tpo(get_page_from_line_address((int)line), &t, &p);
  *(int *)get_pageinpte_address(t, p) = (int)phy;
}

// 映射地址
void page_map(void *target, void *start, void *end) {
  target   = (void *)((size_t)target & 0xfffff000);
  start    = (void *)((size_t)start & 0xfffff000);
  end      = (void *)((size_t)end & 0xfffff000);
  size_t n = end - start;
  n        = n / 4 * 1024;
  for (u32 i = 0; i <= n; i++) {
    var tmp1 = phy_addr_from_linear_addr(target + i * 4 * 1024);
    var tmp2 = phy_addr_from_linear_addr(start + i * 4 * 1024);
    set_phy_addr_of_linear_addr(target + i * 4 * 1024, tmp2);
    set_phy_addr_of_linear_addr(start + i * 4 * 1024, tmp1);
  }
}

u32 page_get_attr(u32 pde, u32 vaddr) {
  pde       += (u32)(PDI(vaddr) * 4);
  void *pte  = (void *)(*(u32 *)pde & 0xfffff000);
  pte       += (u32)(PTI(vaddr) * 4);
  return (*(u32 *)pte) & 0x00000fff;
}

u32 page_get_phy(u32 pde, u32 vaddr) {
  pde       += (u32)(PDI(vaddr) * 4);
  void *pte  = (void *)(*(u32 *)pde & 0xfffff000);
  pte       += (u32)(PTI(vaddr) * 4);
  return (*(u32 *)pte) & 0xfffff000;
}

void copy_on_write(u32 vaddr) {
  void *pd      = (void *)current_task->cr3;                 // PDE页目录地址
  void *pde     = (void *)((u32)pd + (u32)(PDI(vaddr) * 4)); // PTE地址
  void *pde_phy = (void *)(*(u32 *)(pde) & 0xfffff000);      // 页

  if (!(*(u32 *)pde & PAGE_WRABLE) || !(*(u32 *)pde & PAGE_USER)) { // PDE如果不可写
    // 不可写的话，就需要对PDE做COW操作
    u32 backup = *(u32 *)(pde); // 用于备份原有页的属性
    if (pages[IDX(backup)].count < 2 || *(u32 *)pde & PAGE_SHARED) {
      // 如果只有一个人引用，并且PDE属性是共享
      // 设置可写属性，然后进入下一步
      *(u32 *)pde |= PAGE_WRABLE;
      *(u32 *)pde |= PAGE_USER;
      *(u32 *)pde |= PAGE_PRESENT;
      goto PDE_FLUSH;
    }
    // 进行COW
    *(u32 *)(pde) = (u32)page_malloc_one_count_from_4gb(); // 分配一页
    memcpy((void *)(*(u32 *)pde), pde_phy, PAGE_SIZE);     // 复制内容
    *(u32 *)(pde) |=
        (backup & 0x00000fff) | PAGE_WRABLE | PAGE_USER | PAGE_PRESENT; // 设置属性（并且可读）
    pages[IDX(backup)].count--;                                         // 原有引用减少
  PDE_FLUSH:
    // 刷新快表
    flush_tlb(*(u32 *)(pde));
  }
  void *pt  = (void *)(*(u32 *)pde & 0xfffff000);
  void *pte = pt + (u32)(PTI(vaddr) * 4);
  if (!(*(u32 *)pte & PAGE_WRABLE)) {
    if (pages[IDX(*(u32 *)pte)].count < 2 || // 只有一个人引用
        *(u32 *)pte & PAGE_SHARED /*或   这是一个SHARED页*/) {
      *(u32 *)pte |= PAGE_WRABLE; // 设置RWW
      goto FLUSH;
    }
    // 获取旧页信息
    u32   old_pte = *(u32 *)pte;
    void *phy     = (void *)(old_pte & 0xfffff000);

    // 分配一个页
    //  klogd("UPDATE %08x\n", vaddr);
    void *new_page = page_malloc_one_count_from_4gb();
    memcpy(new_page, phy, PAGE_SIZE);

    // 获取原先页的属性
    u32 attr = old_pte & 0x00000fff;

    // 设置PWU
    attr = attr | PAGE_WRABLE;

    // 计算新PTE
    u32 new_pte  = 0;
    new_pte     |= ((u32)new_page) & 0xfffff000;
    new_pte     |= attr;

    // 设置并更新
    pages[IDX(old_pte)].count--;
    *(u32 *)pte = new_pte;
  FLUSH:
    // 刷新TLB快表
    flush_tlb((u32)pte);
  }
  flush_tlb((u32)vaddr);
}

// 设置页属性和物理地址
void page_set_physics_attr(u32 vaddr, void *paddr, u32 attr) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(vaddr);
  p        = (vaddr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((cr3_backup + t * 4));
  if (pages[IDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) { // 这里SHARED页就不进行COW操作
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4);

  if (pages[IDX(*physics)].count > 1) { pages[IDX(*physics)].count--; }
  *physics  = (u32)paddr;
  *physics |= attr;
  flush_tlb((u32)pte);
  flush_tlb(vaddr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

void page_set_physics_attr_pde(u32 vaddr, void *paddr, u32 attr, u32 cr3_backup) {
  u32 t, p;
  t        = PDI(vaddr);
  p        = (vaddr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((cr3_backup + t * 4));
  if (pages[IDX(*pte)].count > 1) { // 这里SHARED页就不进行COW操作
    pages[IDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4);

  // if (pages[IDX(*physics)].count > 1) {
  //   pages[IDX(*physics)].count--;
  // }
  *physics  = (u32)paddr;
  *physics |= attr;
  flush_tlb((u32)pte);
  flush_tlb(vaddr);
}

__attr(fastcall) void page_fault(i32 id, regs32 *regs) {
  asm_cli;
  u32 pde = current_task->cr3;
  asm_set_cr3(PD_ADDRESS); // 设置一个安全的页表

  void *line_address = (void *)asm_get_cr2();
  if (!(page_get_attr(pde, (u32)line_address) & PAGE_PRESENT) || // 不存在
      (!(page_get_attr(pde, (u32)line_address) & PAGE_USER))) {  // 用户不可写
    error("Attempt to read/write a non-existent/kernel memory %p at %p.", line_address, regs->eip);
    if (current_task->user_mode) {
      task_abort();
    } else {
      abort(); // 系统级FAULT
    }
  }
  copy_on_write((u32)line_address);

  asm_set_cr3(pde);
  asm_cli;
}

void page_set_attr(u32 start, u32 end, u32 attr, u32 pde) {
  int count = PADDING_UP(end - start, PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < count; i++) {
    u32 *pde_entry  = (u32 *)(pde + PDI(start + i * PAGE_SIZE) * 4);
    u32  p          = *pde_entry & (0xfffff000);
    u32 *pte_entry  = (u32 *)(p + PTI(start + i * PAGE_SIZE) * 4);
    *pte_entry     |= attr;
  }
  asm_set_cr3(pde);
}

//* ----------------------------------------------------------------------------------------------------
//; 内存权限检查
//* ----------------------------------------------------------------------------------------------------

bool check_address_permission(const void *addr, bool wr) {
  size_t cr3 = asm_get_cr3();
  size_t pde = mem_get(cr3 + PDI(addr) * 4);
  if (!(pde & PAGE_PRESENT)) return false;
  if (wr && !(pde & PAGE_WRABLE)) return false;
  if (!(pde & PAGE_USER)) return false;
  size_t pte = mem_get((pde & 0xfffff000) + PTI(addr) * 4);
  if (!(pte & PAGE_PRESENT)) return false;
  if (wr && !(pte & PAGE_WRABLE)) return false;
  if (!(pte & PAGE_USER)) return false;
  return true;
}

bool check_memory_permission(const void *_addr, size_t size, bool wr) {
  const void *addr     = (void *)PADDING_DOWN(_addr, PAGE_SIZE);
  const void *end_addr = _addr + size;
  for (; addr < end_addr; addr += PAGE_SIZE) {
    if (!check_address_permission(addr, wr)) return false;
  }
  return true;
}

bool check_string_permission(cstr addr) {
  if (!check_address_permission(addr, false)) return false;
  for (addr++;; addr++) {
    if (!((size_t)addr & 0xfff) && !check_address_permission(addr, false)) return false;
    if (*addr == '\0') break;
  }
  return true;
}
