#pragma once
#include <define.h>

#define PAGE_P      MASK(0)
#define PAGE_WRABLE MASK(1)
#define PAGE_USER   MASK(2)
#define PAGE_WT     MASK(3) // 使用直写而不是写回
#define PAGE_CD     MASK(4) // 禁用缓存
#define PAGE_SHARED 1024    // 自定义的

#define PDE_ADDRESS   0x400000
#define PTE_ADDRESS   (PDE_ADDRESS + 0x1000)
#define PAGE_END      (PTE_ADDRESS + 0x400000)
#define PAGE_MANNAGER PAGE_END
#define NULL_TID      11459810

typedef struct __PACKED__ PageInfo {
  u8 task_id;
  u8 count;
} PageInfo;

/**
 *\brief 构建线性地址
 *
 *\param table    页目录地址
 *\param page     页表地址
 *\param off      页内偏移地址
 *\return 线性地址
 */
finline size_t mk_linear_addr(size_t table, size_t page, size_t off) {
  return (table << 22) + (page << 12) + off;
}

u32   page_get_attr(u32 vaddr);
u32   page_get_attr_pde(u32 vaddr, u32 pde);
u32   page_get_phy_pde(u32 vaddr, u32 pde);
void  tpo2page(int *page, int t, int p);
void *page_malloc_one_count_from_4gb();
void *page_alloc(size_t size);
void  page_free(void *p, size_t size);
void  task_free_all_pages(u32 tid);
void  change_page_task_id(int task_id, void *p, u32 size);
u32   pde_clone(u32 addr);
void *page_malloc_one();
void *page_malloc_one_no_mark();
void  page_link(u32 addr);
void  page_link_share(u32 addr);
void  page_unlink(u32 addr);
u32   page_get_alloced();
void  page_link_addr_pde(u32 addr, u32 pde, u32 map_addr);
