#include <kernel.h>

#define IDX(addr)  ((u32)(addr) >> 12)           // 获取 addr 的页索引
#define DIDX(addr) (((u32)(addr) >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define TIDX(addr) (((u32)(addr) >> 12) & 0x3ff) // 获取 addr 的页表索引
#define PAGE(idx)  ((u32)(idx) << 12)            // 获取页索引 idx 对应的页开始的位置

// 刷新虚拟地址 vaddr 的 块表 TLB
static void flush_tlb(size_t vaddr) {
  asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

PageInfo *pages = (PageInfo *)PAGE_MANNAGER;

void init_pdepte(size_t pde_addr, size_t pte_addr, size_t page_end) {
  memset((void *)pde_addr, 0, page_end - pde_addr);
  { // 这是初始化 PDE (页目录)
    size_t *addr = (void *)pde_addr;
    for (size_t i = 0; addr + i < (size_t *)pte_addr; i++) {
      addr[i] = (pte_addr + i * PAGE_SIZE) | PAGE_P | PAGE_WRABLE;
    }
  }

  { // 这是初始化 PTE (页表)
    size_t *addr = (void *)PTE_ADDRESS;
    for (size_t i = 0; addr + i < (size_t *)PAGE_END; i++) {
      addr[i] = (i * PAGE_SIZE) | PAGE_P | PAGE_WRABLE;
    }
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
u32 pde_clone(u32 addr) {
  for (int i = DIDX(0x70000000) * 4; i < PAGE_SIZE; i += 4) {
    u32 *pde_entry = (u32 *)(addr + i);
    u32  p         = *pde_entry & (0xfffff000);
    pages[IDX(*pde_entry)].count++;
    *pde_entry &= ~PAGE_WRABLE;
    for (int j = 0; j < PAGE_SIZE; j += 4) {
      u32 *pte_entry = (u32 *)(p + j);
      if ((page_get_attr(mk_linear_addr(i / 4, j / 4, 0)) & PAGE_USER)) {
        pages[IDX(*pte_entry)].count++;
        if (page_get_attr(mk_linear_addr(i / 4, j / 4, 0)) & PAGE_SHARED) {
          *pte_entry |= PAGE_WRABLE;
          continue;
        }
      }
      *pte_entry &= ~PAGE_WRABLE;
    }
  }
  u32 result = (u32)page_malloc_one_no_mark();
  memcpy((void *)result, (void *)addr, PAGE_SIZE);
  flush_tlb(result);
  flush_tlb(addr);
  asm_set_cr3(addr);

  return result;
}

void pde_reset(u32 addr) {
  for (int i = DIDX(0x70000000) * 4; i < PAGE_SIZE; i += 4) {
    u32 *pde_entry  = (u32 *)(addr + i);
    *pde_entry     |= PAGE_WRABLE;
  }
}

void free_pde(u32 addr) {
  if (addr == PDE_ADDRESS) return;
  for (int i = DIDX(0x70000000) * 4; i < DIDX(0xf1000000) * 4; i += 4) {
    u32 *pde_entry = (u32 *)(addr + i);
    u32  p         = *pde_entry & (0xfffff000);
    if (!(*pde_entry & PAGE_USER) && !(*pde_entry & PAGE_P)) { continue; }
    for (int j = 0; j < PAGE_SIZE; j += 4) {
      u32 *pte_entry = (u32 *)(p + j);
      if (*pte_entry & PAGE_USER && *pte_entry & PAGE_P) { pages[IDX(*pte_entry)].count--; }
    }

    pages[IDX(*pde_entry)].count--;
  }
  flush_tlb(addr);
  page_free((void *)addr, PAGE_SIZE);
}

void page_link_pde(u32 addr, u32 pde) {
  u32 pde_backup      = current_task()->pde;
  current_task()->pde = PDE_ADDRESS;
  asm_set_cr3(PDE_ADDRESS);
  u32 t, p;
  t        = DIDX(addr);
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
  *physics  = (u32)page_malloc_one_count_from_4gb();
  *physics |= 7;
  flush_tlb((u32)pte);
  flush_tlb(addr);
  current_task()->pde = pde_backup;
  asm_set_cr3(pde_backup);
}
void page_link_addr_pde(u32 addr, u32 pde, u32 map_addr) {
  u32 pde_backup      = current_task()->pde;
  current_task()->pde = PDE_ADDRESS;
  asm_set_cr3(PDE_ADDRESS);
  u32 t, p;
  t        = DIDX(addr);
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
  current_task()->pde = pde_backup;
  asm_set_cr3(pde_backup);
}

void page_link_pde_share(u32 addr, u32 pde) {
  u32 pde_backup      = current_task()->pde;
  current_task()->pde = PDE_ADDRESS;
  asm_set_cr3(PDE_ADDRESS);
  u32 t, p;
  t        = DIDX(addr);
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
  current_task()->pde = pde_backup;
  asm_set_cr3(pde_backup);
}

void page_link_pde_paddr(u32 addr, u32 pde, u32 *paddr1, u32 paddr2) {
  u32 pde_backup      = current_task()->pde;
  current_task()->pde = PDE_ADDRESS;
  asm_set_cr3(PDE_ADDRESS);
  u32 t, p;
  t        = DIDX(addr);
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
  current_task()->pde = pde_backup;
  asm_set_cr3(pde_backup);
}

u32 page_get_alloced() {
  u32 r = 0;
  for (int i = 0; i < PADDING_UP(total_mem_size, PAGE_SIZE) / PAGE_SIZE; i++) {
    if (pages[i].count) { r++; }
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
      u32 addr         = PAGE(i);
      pages[i].task_id = get_tid(current_task());
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
  page_links_pde(start, numbers, current_task()->pde);
}

void page_link(u32 addr) {
  page_link_pde(addr, current_task()->pde);
}

void page_link_share(u32 addr) {
  page_link_pde_share(addr, current_task()->pde);
}

void copy_from_phy_to_line(u32 phy, u32 line, u32 pde, u32 size) {
  size_t pg = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < pg; i++) {
    memcpy((void *)page_get_phy_pde(line, pde), (void *)phy, min(size, PAGE_SIZE));
    size -= PAGE_SIZE;
    line += PAGE_SIZE;
    phy  += PAGE_SIZE;
  }
}

void set_line_address(u32 val, u32 line, u32 pde, u32 size) {
  size_t pg = PADDING_UP(size, PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < pg; i++) {
    memset((void *)page_get_phy_pde(line, pde), val, min(size, PAGE_SIZE));
    size -= PAGE_SIZE;
    line += PAGE_SIZE;
  }
}

void page_unlink_pde(u32 addr, u32 pde) {
  u32 pde_backup      = current_task()->pde;
  current_task()->pde = PDE_ADDRESS;
  asm_set_cr3(PDE_ADDRESS);
  u32 t, p;
  t        = DIDX(addr);
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
  current_task()->pde = pde_backup;
  asm_set_cr3(pde_backup);
}

void page_unlink(u32 addr) {
  page_unlink_pde(addr, current_task()->pde);
}

void init_page() {
  init_pdepte(PDE_ADDRESS, PTE_ADDRESS, PAGE_END);
  page_manager_init(pages);
  // kernel 加载到0x280000
  // 保留 0 到 0xb00000 (不含)
  page_set_alloced(pages, 0, 0xb00000);
  // 0xc0000000 到 0xffffffff 物理内存保留地址
  page_set_alloced(pages, 0xc0000000, 0xffffffff);
  asm_set_cr3(PDE_ADDRESS);
  asm_set_pg;
}

void pf_set(u32 memsize) {
  u32 *pte = (u32 *)PTE_ADDRESS;
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

void *page_malloc_one_no_mark() {
  int i;
  for (i = 0; i != 1024 * 1024; i++) {
    if (pages[i].count == 0) {
      int t, p;
      page2tpo(i, &t, &p);
      u32 addr         = mk_linear_addr(t, p, 0);
      pages[i].task_id = 0;
      pages[i].count++;
      return (void *)addr;
    }
  }
  return NULL;
}

void *page_malloc_one() {
  for (int i = 0; i != 1024 * 1024; i++) {
    if (pages[i].count == 0) {
      int t, p;
      page2tpo(i, &t, &p);
      u32 addr         = mk_linear_addr(t, p, 0);
      pages[i].task_id = get_tid(current_task());
      pages[i].count++;
      return (void *)addr;
    }
  }
  return NULL;
}

void *page_malloc_one_mark(u32 tid) {
  int i;
  for (i = 0; i != 1024 * 1024; i++) {
    if (pages[i].count == 0) {
      int t, p;
      page2tpo(i, &t, &p);
      u32 addr         = mk_linear_addr(t, p, 0);
      pages[i].task_id = tid;
      pages[i].count++;
      return (void *)addr;
    }
  }

  return NULL;
}

void *page_malloc_one_count_from_4gb() {
  for (int i = IDX(total_mem_size) - 1; i >= 0; i--) {
    if (pages[i].count == 0) {
      u32 addr         = PAGE(i);
      pages[i].task_id = get_tid(current_task());
      pages[i].count++;
      return (void *)addr;
    }
  }
  return NULL;
}

void task_free_all_pages(u32 tid) {
  for (int i = 0; i < 1024 * 1024; i++) {
    if (pages[i].count && pages[i].task_id == tid) {
      pages[i].count   = 0;
      pages[i].task_id = 0;
    }
  }
}

int get_pageinpte_address(int t, int p) {
  int page;
  tpo2page(&page, t, p);
  return (PTE_ADDRESS + page * 4);
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
        pages[j].task_id = 0;
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
    if (id >= 1024 * 1024) fatal("炸啦！");                                 // 超过最大页
    if (pages[id].count <= 1 || pages[id].task_id == current_task()->tid) { //
      pages[id].task_id = 0;
    }
    pages[id].count--;
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

void change_page_task_id(int task_id, void *p, u32 size) {
  int page = get_page_from_line_address((int)p);
  for (int i = 0; i != ((size - 1) / (4 * 1024)) + 1; i++) {
    pages[page + i].task_id = task_id;
  }
}

u32 page_get_attr_pde(u32 vaddr, u32 pde) {
  pde       += (u32)(DIDX(vaddr) * 4);
  void *pte  = (void *)(*(u32 *)pde & 0xfffff000);
  pte       += (u32)(TIDX(vaddr) * 4);
  return (*(u32 *)pte) & 0x00000fff;
}

u32 page_get_attr(u32 vaddr) {
  return page_get_attr_pde(vaddr, current_task()->pde);
}

u32 page_get_phy_pde(u32 vaddr, u32 pde) {
  pde       += (u32)(DIDX(vaddr) * 4);
  void *pte  = (void *)(*(u32 *)pde & 0xfffff000);
  pte       += (u32)(TIDX(vaddr) * 4);
  return (*(u32 *)pte) & 0xfffff000;
}

u32 page_get_phy(u32 vaddr) {
  return page_get_phy_pde(vaddr, current_task()->pde);
}

void copy_on_write(u32 vaddr) {
  void *pd      = (void *)current_task()->pde;                // PDE页目录地址
  void *pde     = (void *)((u32)pd + (u32)(DIDX(vaddr) * 4)); // PTE地址
  void *pde_phy = (void *)(*(u32 *)(pde) & 0xfffff000);       // 页

  if (!(*(u32 *)pde & PAGE_WRABLE) || !(*(u32 *)pde & PAGE_USER)) { // PDE如果不可写
    // 不可写的话，就需要对PDE做COW操作
    u32 backup = *(u32 *)(pde); // 用于备份原有页的属性
    if (pages[IDX(backup)].count < 2 || *(u32 *)pde & PAGE_SHARED) {
      // 如果只有一个人引用，并且PDE属性是共享
      // 设置可写属性，然后进入下一步
      *(u32 *)pde |= PAGE_WRABLE;
      *(u32 *)pde |= PAGE_USER;
      *(u32 *)pde |= PAGE_P;
      goto PDE_FLUSH;
    }
    // 进行COW
    *(u32 *)(pde) = (u32)page_malloc_one_count_from_4gb(); // 分配一页
    memcpy((void *)(*(u32 *)pde), pde_phy, PAGE_SIZE);     // 复制内容
    *(u32 *)(pde) |=
        (backup & 0x00000fff) | PAGE_WRABLE | PAGE_USER | PAGE_P; // 设置属性（并且可读）
    pages[IDX(backup)].count--;                                   // 原有引用减少
  PDE_FLUSH:
    // 刷新快表
    flush_tlb(*(u32 *)(pde));
  } else {
  }
  void *pt  = (void *)(*(u32 *)pde & 0xfffff000);
  void *pte = pt + (u32)(TIDX(vaddr) * 4);
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
  u32 pde_backup      = current_task()->pde;
  current_task()->pde = PDE_ADDRESS;
  asm_set_cr3(PDE_ADDRESS);
  u32 t, p;
  t        = DIDX(vaddr);
  p        = (vaddr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde_backup + t * 4));
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
  current_task()->pde = pde_backup;
  asm_set_cr3(pde_backup);
}

void page_set_physics_attr_pde(u32 vaddr, void *paddr, u32 attr, u32 pde_backup) {
  u32 t, p;
  t        = DIDX(vaddr);
  p        = (vaddr >> 12) & 0x3ff;
  u32 *pte = (u32 *)((pde_backup + t * 4));
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

extern TSS32 tss;

void PF(u32 edi, u32 esi, u32 ebp, u32 esp, u32 ebx, u32 edx, u32 ecx, u32 eax, u32 gs, u32 fs,
        u32 es, u32 ds, u32 error, u32 eip, u32 cs, u32 eflags) {
  u32 pde = current_task()->pde;
  asm_cli;
  asm_set_cr3(PDE_ADDRESS); // 设置一个安全的页表
  void *line_address = (void *)asm_get_cr2();
  if (!(page_get_attr((u32)line_address) & PAGE_P) ||      // 不存在
      (!(page_get_attr((u32)line_address) & PAGE_USER))) { // 用户不可写
    error("Attempt to read/write a non-existent/kernel memory %p at %p. System halt.", line_address,
          eip);
    if (current_task()->user_mode) { // 用户级FAULT
      task_exit(-1);                 // 强制退出
      infinite_loop;
    }
    asm_cli;
    abort(); // 系统级FAULT
  }
  copy_on_write((u32)line_address);
  asm_set_cr3(pde);
  asm_cli;
}

void page_set_attr(u32 start, u32 end, u32 attr, u32 pde) {
  int count = PADDING_UP(end - start, PAGE_SIZE) / PAGE_SIZE;
  for (int i = 0; i < count; i++) {
    u32 *pde_entry  = (u32 *)(pde + DIDX(start + i * PAGE_SIZE) * 4);
    u32  p          = *pde_entry & (0xfffff000);
    u32 *pte_entry  = (u32 *)(p + TIDX(start + i * PAGE_SIZE) * 4);
    *pte_entry     |= attr;
  }
  asm_set_cr3(pde);
}
