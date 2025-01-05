#include <kernel.h>

#define PIDX(addr) ((usize)(addr) >> 12)           // 获取 addr 的页索引
#define PDI(addr)  (((usize)(addr) >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define PTI(addr)  (((usize)(addr) >> 12) & 0x3ff) // 获取 addr 的页表索引
#define PADDR(idx) ((usize)(idx) << 12)            // 获取页索引 idx 对应的页开始的位置

#define pdeof(addr, pd) ((PDE *)(pd) + PDI(addr))
#define pteof(addr, pt) ((PTE *)(pt) + PTI(addr))

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

void page_manager_init(PageInfo *pg) { // 全部置为0就好
  memset(pg, 0, 1024 * 1024 * sizeof(PageInfo));
}

void page_set_alloced(PageInfo *pg, u32 start, u32 end) {
  for (usize i = PIDX(start); i <= PIDX(end); i++) {
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
  val pd = (PDE *)addr;
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
  val result = page_malloc_one();
  memcpy(result, (void *)addr, PAGE_SIZE);
  flush_tlb(result);
  flush_tlb(addr);
  asm_set_cr3(addr);
  return (usize)result;
}

static void pd_reset(u32 addr) {
  for (usize i = PDI(0x70000000) * 4; i < PAGE_SIZE; i += 4) {
    var pde     = (PDE *)(addr + i);
    pde->wrable = true;
  }
}

void pd_free(usize addr) {
  if (addr == PD_ADDRESS) return;
  for (usize i = PDI(0x70000000); i < 1024; i++) {
    val pde = (PDE *)addr + i;
    if (!pde->present || !pde->user) continue;
    for (usize j = 0; j < 1024; j++) {
      val pte = (PTE *)(pde->addr << 12) + j;
      if (!pte->present || !pte->user) continue;
      pages[pte->addr].count--;
    }
    pages[pde->addr].count--;
  }
  flush_tlb(addr);
  page_free((void *)addr, PAGE_SIZE);
}

void page_link2(usize addr, usize pd) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32  t   = PDI(addr);
  u32  p   = PTI(addr);
  PDE *pde = (PDE *)((pd + t * 4));

  // 其实我们应该判断一下这里user能不能写的
  // 但是大多数情况下pde都比较干净，所以先不判断了
  if (pages[pde->addr].count > 1) {
    // 这个页目录还有人引用，所以需要复制
    pages[pde->addr].count--;
    void *old = (void *)PADDR(pde->addr);
    pde->addr = PIDX(page_malloc_one_count_from_4gb());
    memcpy((void *)(PADDR(pde->addr)), old, PAGE_SIZE);
  }
  pde->wrable  = true;
  pde->user    = true;
  pde->present = true;

  PTE *physics = (PTE *)(PADDR(pde->addr) + p * 4); // PTE页表
  // COW
  // 本来就不是给你用户访问的就不需要操作引用计数了
  // 这里不需要大于1,因为我们就相当于是抛弃了原来的页
  if (pages[physics->addr].count && physics->user) { pages[physics->addr].count--; }
  physics->addr    = PIDX(page_malloc_one_count_from_4gb());
  physics->wrable  = true;
  physics->user    = true;
  physics->present = true;
  flush_tlb((void *)pde);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

void page_link_addr_pde(usize addr, usize pd, usize map_addr) {
  u32 cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t, p;
  t        = PDI(addr);
  p        = PTI(addr);
  PDE *pde = (PDE *)((pd + t * 4));

  // 其实我们应该判断一下这里user能不能写的
  // 但是大多数情况下pde都比较干净，所以先不判断了
  if (pages[pde->addr].count > 1) {
    // 这个页目录还有人引用，所以需要复制
    pages[pde->addr].count--;
    void *old = (void *)PADDR(pde->addr);
    pde->addr = PIDX(page_malloc_one_count_from_4gb());
    memcpy((void *)(PADDR(pde->addr)), old, PAGE_SIZE);
  }
  pde->wrable  = true;
  pde->user    = true;
  pde->present = true;

  var physics = (PTE *)(PADDR(pde->addr) + p * 4); // PTE页表

  // COW
  // 这里不需要大于1,因为我们就相当于是抛弃了原来的页
  if (pages[physics->addr].count && physics->user) pages[physics->addr].count--;
  pages[PIDX(map_addr)].count++;
  physics->addr    = PIDX(map_addr);
  physics->wrable  = true;
  physics->user    = true;
  physics->present = true;
  flush_tlb((void *)pde);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

// share 和 没有share的区别是
// share会处理PAGE_SHARED标志，如果这个PDE已经被多个进程引用，那么则会直接覆写
// 而没有share遇到PAGE_SHARED标志，那么如果这个PDE已经被多个进程引用，那么会复制一份
void page_link_share2(usize addr, usize pde) {
  usize cr3_backup  = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32  t   = PDI(addr);
  u32  p   = (addr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde + t * 4));

  if (pages[PIDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) {
    // 这个页目录还有人引用，所以需要复制
    pages[PIDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4); // PTE页表
  // COW
  if (pages[PIDX(*physics)].count > 1) { pages[PIDX(*physics)].count--; }
  int flag = 0;
  if (*physics & PAGE_SHARED) {
    klogd("THIS\n");
    flag = 1;
  }
  *physics = (u32)page_malloc_one_count_from_4gb() | 7;
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
  if (pages[PIDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) {
    int flag;
    if (*pte & PAGE_SHARED) flag = 1;
    pages[PIDX(*pte)].count--;
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
  if (pages[PIDX(*physics)].count > 1) { pages[PIDX(*physics)].count--; }
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

void page_link1(u32 addr) {
  page_link2(addr, current_task->cr3);
}

void page_link_share1(u32 addr) {
  page_link_share2(addr, current_task->cr3);
}

void copy_from_phy_to_line(u32 phy, u32 line, u32 pde, u32 size) {
  val pg = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  for (usize i = 0; i < pg; i++) {
    memcpy((void *)page_get_phy(line, pde), (void *)phy, min(size, PAGE_SIZE));
    size -= PAGE_SIZE;
    line += PAGE_SIZE;
    phy  += PAGE_SIZE;
  }
}

void set_line_address(u32 value, u32 line, u32 pde, u32 size) {
  val pg = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  for (usize i = 0; i < pg; i++) {
    memset((void *)page_get_phy(line, pde), value, min(size, PAGE_SIZE));
    size -= PAGE_SIZE;
    line += PAGE_SIZE;
  }
}

void page_unlink_pd(usize addr, usize pd) {
  val cr3_backup    = current_task->cr3;
  current_task->cr3 = PD_ADDRESS;
  asm_set_cr3(PD_ADDRESS);
  u32 t   = PDI(addr);
  u32 p   = (addr >> 12) & 0x3ff;
  var pde = pdeof(addr, pd);

  if (pages[pde->addr].count > 1 && !pde->shared) {
    // 这个页目录还有人引用，所以需要复制
    pages[pde->addr].count--;
    usize old     = pde->addr;
    void *alloced = page_malloc_one_count_from_4gb();
    memcpy(alloced, (void *)old, PAGE_SIZE);
    pde->addr    = (usize)alloced >> 12;
    pde->present = true;
    pde->wrable  = true;
    pde->user    = true;
  }

  var pte = pteof(addr, pde->addr << 12);
  if (pages[pte->addr].count && pte->user) pages[pte->addr].count--;
  pte->present = false;
  flush_tlb((u32)pde);
  flush_tlb(addr);
  current_task->cr3 = cr3_backup;
  asm_set_cr3(cr3_backup);
}

void page_unlink(u32 addr) {
  page_unlink_pd(addr, current_task->cr3);
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
  for (usize i = 0; i != 1024 * 1024; i++) {
    if (pages[i].count == 0) {
      int t, p;
      page2tpo(i, &t, &p);
      u32 addr = mk_linear_addr(t, p, 0);
      pages[i].count++;
      return (void *)addr;
    }
  }
  return null;
}

void *page_malloc_one_count_from_4gb() {
  for (usize i = PIDX(total_mem_size) - 1; i >= 0; i--) {
    if (pages[i].count == 0) {
      u32 addr = PADDR(i);
      pages[i].count++;
      return (void *)addr;
    }
  }
  return null;
}

int get_pageinpte_address(int t, int p) {
  int page;
  tpo2page(&page, t, p);
  return (PT_ADDRESS + page * 4);
}

static usize page_alloc_line_cache = 0;

static vectorize isize find_kpage(usize line, usize n) {
  line       = max(line, page_alloc_line_cache);
  usize free = 0;
  for (; line < 1024 * 1024; line++) {
    if (pages[line].count != 0) {
      free = 0;
      continue;
    }
    if (++free == n) {
      val begin = line - n + 1;
      for (usize j = begin; j <= line; j++) {
        pages[j].count++;
      }
      page_alloc_line_cache = line + 1;
      return begin;
    }
  }
  page_alloc_line_cache = SIZE_MAX;
  return -1;
}

void *page_alloc(usize size) {
  val npages = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  val idx    = find_kpage(0, npages);
  if (idx < 0) return null;
  int t, p;
  page2tpo(idx, &t, &p);
  val addr = (void *)mk_linear_addr(t, p, 0);
  memset(addr, 0, npages * PAGE_SIZE);
  return addr;
}

void page_free(void *ptr, usize size) {
  kassert(((usize)ptr & (PAGE_SIZE - 1)) == 0);
  page_alloc_line_cache = min(page_alloc_line_cache, (usize)ptr);
  for (size_t id = (size_t)ptr / PAGE_SIZE, i = 0; i < size; i += PAGE_SIZE, id++) {
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

usize page_get_attr2(usize addr, usize pd) {
  var pde = pdeof(addr, pd);
  var pte = pteof(addr, pde->addr << 12);
  return *(usize *)pte & 0xfff;
}

usize page_get_attr1(usize addr) {
  return page_get_attr2(addr, asm_get_cr3());
}

usize page_get_phy2(usize addr, usize pd) {
  var pde = pdeof(addr, pd);
  var pte = pteof(addr, pde->addr << 12);
  return pte->addr << 12;
}

usize page_get_phy1(usize addr) {
  return page_get_phy2(addr, asm_get_cr3());
}

void copy_on_write(u32 vaddr) {
  void *pd      = (void *)current_task->cr3;                 // PDE页目录地址
  void *pde     = (void *)((u32)pd + (u32)(PDI(vaddr) * 4)); // PTE地址
  void *pde_phy = (void *)(*(u32 *)(pde) & 0xfffff000);      // 页

  if (!(*(u32 *)pde & PAGE_WRABLE) || !(*(u32 *)pde & PAGE_USER)) { // PDE如果不可写
    // 不可写的话，就需要对PDE做COW操作
    u32 backup = *(u32 *)(pde); // 用于备份原有页的属性
    if (pages[PIDX(backup)].count < 2 || *(u32 *)pde & PAGE_SHARED) {
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
    pages[PIDX(backup)].count--;                                        // 原有引用减少
  PDE_FLUSH:
    // 刷新快表
    flush_tlb(*(u32 *)(pde));
  }
  void *pt  = (void *)(*(u32 *)pde & 0xfffff000);
  void *pte = pt + (u32)(PTI(vaddr) * 4);
  if (!(*(u32 *)pte & PAGE_WRABLE)) {
    if (pages[PIDX(*(u32 *)pte)].count < 2 || // 只有一个人引用
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
    pages[PIDX(old_pte)].count--;
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
  if (pages[PIDX(*pte)].count > 1 && !(*pte & PAGE_SHARED)) { // 这里SHARED页就不进行COW操作
    pages[PIDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4);

  if (pages[PIDX(*physics)].count > 1) { pages[PIDX(*physics)].count--; }
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
  if (pages[PIDX(*pte)].count > 1) { // 这里SHARED页就不进行COW操作
    pages[PIDX(*pte)].count--;
    u32 old = *pte & 0xfffff000;
    *pte    = (u32)page_malloc_one_count_from_4gb();
    memcpy((void *)(*pte), (void *)old, PAGE_SIZE);
    *pte |= 7;
  } else {
    *pte |= 7;
  }

  u32 *physics = (u32 *)((*pte & 0xfffff000) + p * 4);

  // if (pages[PIDX(*physics)].count > 1) {
  //   pages[PIDX(*physics)].count--;
  // }
  *physics  = (u32)paddr;
  *physics |= attr;
  flush_tlb((u32)pte);
  flush_tlb(vaddr);
}

FASTCALL void page_fault(i32 id, regs32 *regs) {
  u32 pd = current_task->cr3;
  asm_set_cr3(PD_ADDRESS); // 设置一个安全的页表

  val addr = asm_get_cr2();

  var pde = pdeof(addr, pd);
  var pte = pteof(addr, pde->addr << 12);

  if (!pte->present || !pte->user) {
    info("task %d", current_task->tid);
    error("Attempt to %s a %s memory %p at %p.",              //
          regs->err & PF_WRITE ? "write" : "read",            //
          regs->err & PF_PRESENT ? "kernel" : "non-existent", //
          addr, regs->eip);
    if (regs->err & PF_USER) {
      kassert(current_task->user_mode);
      task_abort();
    } else {
      abort();
    }
  }

  copy_on_write(addr);

  asm_set_cr3(pd);
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

bool check_address_permission3(const void *addr, bool wr, usize cr3) {
  var pde = pdeof(addr, cr3);
  if (!pde->present) return false;
  if (wr && !pde->wrable) return false;
  if (!pde->user) return false;
  var pte = pteof(addr, pde->addr << 12);
  if (!pte->present) return false;
  if (wr && !pte->wrable) return false;
  if (!pte->user) return false;
  return true;
}

bool check_address_permission2(const void *addr, bool wr) {
  return check_address_permission3(addr, wr, asm_get_cr3());
}

bool check_address_permission1(const void *addr) {
  return check_address_permission3(addr, false, asm_get_cr3());
}

// --------------------------------------------------

bool check_memory_permission4(const void *_addr, size_t size, bool wr, usize cr3) {
  const void *addr     = (void *)PADDING_DOWN(_addr, PAGE_SIZE);
  const void *end_addr = _addr + size;
  for (; addr < end_addr; addr += PAGE_SIZE) {
    if (!check_address_permission(addr, wr, cr3)) return false;
  }
  return true;
}

bool check_memory_permission3(const void *_addr, size_t size, bool wr) {
  return check_memory_permission4(_addr, size, wr, asm_get_cr3());
}

bool check_memory_permission2(const void *_addr, size_t size) {
  return check_memory_permission4(_addr, size, false, asm_get_cr3());
}

// --------------------------------------------------

bool check_string_permission2(cstr addr, usize cr3) {
  if (!check_address_permission(addr, false)) return false;
  for (addr++;; addr++) {
    if (!((size_t)addr & 0xfff) && !check_address_permission(addr, false, cr3)) return false;
    if (*addr == '\0') break;
  }
  return true;
}

bool check_string_permission1(cstr addr) {
  return check_string_permission2(addr, asm_get_cr3());
}
