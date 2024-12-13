#include <kernel.h>

#define IDX(addr)  ((u32)addr >> 12)           // 获取 addr 的页索引
#define DIDX(addr) (((u32)addr >> 22) & 0x3ff) // 获取 addr 的页目录索引
#define TIDX(addr) (((u32)addr >> 12) & 0x3ff) // 获取 addr 的页表索引
#define PAGE(idx)  ((u32)idx << 12)            // 获取页索引 idx 对应的页开始的位置

extern PageInfo *pages;

void *syscall_mmap(void *start, u32 length) {
  // 我们先算出需要占用几个页（对length进行向上取整）
  u32  page_count = PADDING_UP(length, PAGE_SIZE) / PAGE_SIZE;
  bool size_is_2M = page_count == 512;

  u32   addr            = current_task->pde;
  void *line_addr_start = null;
  for (int i = DIDX(0x70000000), c = 0; i < 1024; i++) {
    u32 *pde_entry = (u32 *)addr + i;
    u32  p         = *pde_entry & (0xfffff000);
    for (int j = 0; j < 1024; size_is_2M ? j += 512 : j++) {
      u32 *pte_entry = (u32 *)p + j;
      if (!(page_get_attr(mk_linear_addr(i, j, 0)) & PAGE_WRABLE)) {
        if (c == 0) line_addr_start = (void *)mk_linear_addr(i, j, 0);
        c++;
      } else {
        c = 0;
      }
      if (c == page_count) goto _1;
    }
  }
_1:
  klogd("找到了一段空闲的没有被映射的线性地址%p-%p", line_addr_start,
        line_addr_start + page_count * PAGE_SIZE);
  for (int i = 0; i < page_count; i++) {
    page_link_share((size_t)line_addr_start + i * PAGE_SIZE);
  }
  return line_addr_start;
}

void syscall_munmap(void *start, u32 length) {
  // 我们先算出需要占用几个页（对length进行向上取整）
  u32 page_count = PADDING_UP(length, PAGE_SIZE) / PAGE_SIZE;

  if (start > (void *)0xf0000000) {
    error("Couldn't unmap memory from %p to %p.", start, start + page_count * PAGE_SIZE);
    syscall_exit(-1);
    return;
  }
  for (int i = 0; i < page_count; i++) {
    page_unlink((u32)start + i * PAGE_SIZE);
  }

  klogd("释放了地址%p-%p", start, start + length);
}
