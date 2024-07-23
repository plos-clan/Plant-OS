#pragma once
#include <define.h>
#include <type.h>
#define PG_P          1
#define PG_USU        4
#define PG_RWW        2
#define PG_PCD        16
#define PG_SHARED     1024
#define PDE_ADDRESS   0x400000
#define PTE_ADDRESS   (PDE_ADDRESS + 0x1000)
#define PAGE_END      (PTE_ADDRESS + 0x400000)
#define PAGE_MANNAGER PAGE_END
#define NULL_TID      11459810
struct PAGE_INFO {
  u8 task_id;
  u8 count;
} __PACKED__;
int   get_line_address(int t, int p, int o);
u32   page_get_attr(unsigned vaddr);
u32   page_get_attr_pde(u32 vaddr, u32 pde);
u32   page_get_phy_pde(u32 vaddr, u32 pde);
void  tpo2page(int *page, int t, int p);
void  page_free_one(void *p);
void *page_malloc_one_count_from_4gb();
void *page_malloc(int size);
void  page_free(void *p, int size);
void  gc(unsigned tid);
void  change_page_task_id(int task_id, void *p, u32 size);
u32   pde_clone(u32 addr);
void *page_malloc_one();