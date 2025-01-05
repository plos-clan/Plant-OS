#pragma once
#include "assert.h"
#include <define.h>

#define PAGE_PRESENT MASK(0)  // 存在
#define PAGE_WRABLE  MASK(1)  // 可写
#define PAGE_USER    MASK(2)  // 用户态
#define PAGE_WT      MASK(3)  // 使用直写而不是写回
#define PAGE_CD      MASK(4)  // 禁用缓存
#define PAGE_ACCESS  MASK(5)  // 访问位
#define PAGE_DIRTY   MASK(6)  // 脏页
#define PAGE_GLOBAL  MASK(8)  // 全局页
#define PAGE_SHARED  MASK(10) // 自定义的

#define PD_ADDRESS    0x400000
#define PT_ADDRESS    (PD_ADDRESS + 0x1000)
#define PAGE_END      (PT_ADDRESS + 0x400000)
#define PAGE_MANNAGER PAGE_END

#define PF_PRESENT  MASK(0) // 程序尝试访问的页面是否存在于内存中
#define PF_WRITE    MASK(1) // 程序进行的访问类型是否为写入
#define PF_USER     MASK(2) // 错误是否在用户态下发生
#define PF_RESERVED MASK(3) // 程序是否尝试访问保留的地址
#define PF_IFETCH   MASK(4) // 是否为 cpu 取指时发生的错误

#ifdef __x86_64__

#else

typedef struct PDE {
  u32 present : 1;  // 存在位
  u32 wrable  : 1;  // 读写位
  u32 user    : 1;  // 用户位
  u32 wt      : 1;  // 直写位
  u32 cd      : 1;  // 禁用缓存
  u32 access  : 1;  // 访问位
  u32 dirty   : 1;  // 脏页
  u32 ps      : 1;  // 页大小
  u32 global  : 1;  // 全局页
  u32 bit9    : 1;  //
  u32 shared  : 1;  // 共享页 (自定义的)
  u32 bit11   : 1;  //
  u32 addr    : 20; // 一定要左移 12 位
} PDE;

typedef struct PTE {
  u32 present : 1;  // 存在位
  u32 wrable  : 1;  // 读写位
  u32 user    : 1;  // 用户位
  u32 wt      : 1;  // 直写位
  u32 cd      : 1;  // 禁用缓存
  u32 access  : 1;  // 访问位
  u32 dirty   : 1;  // 脏页
  u32 pat     : 1;  // 页属性表
  u32 global  : 1;  // 全局页
  u32 bit9    : 1;  //
  u32 shared  : 1;  // 共享页 (自定义的)
  u32 bit11   : 1;  //
  u32 addr    : 20; // 一定要左移 12 位
} PTE;

#endif

typedef struct __PACKED__ PageInfo {
  u16 count;
  u16 reserved;
} PageInfo;

#ifdef __x86_64__
/**
 *\brief 构建线性地址 (9 + 9 + 9 + 9 + 12)
 *
 *\param pml4     PML4 地址
 *\param pdpt     PDPT 地址
 *\param pd       PD 地址
 *\param pt       PT 地址
 *\param off      页内偏移地址
 *\return 线性地址
 */
inline_const usize mk_linear_addr(usize pml4, usize pdpt, usize pd, usize pt, usize off) {
  return (pml4 << 39) + (pdpt << 30) + (pd << 21) + (pt << 12) + off;
}
#else
/**
 *\brief 构建线性地址 (10 + 10 + 12)
 *
 *\param table    页目录地址
 *\param page     页表地址
 *\param off      页内偏移地址
 *\return 线性地址
 */
inline_const usize mk_linear_addr(usize table, usize page, usize off) {
  kassert(table < 1024);
  kassert(page < 1024);
  kassert(off < 4096);
  return (table << 22) + (page << 12) + off;
}
#endif

usize page_get_attr2(usize addr, usize pd);
usize page_get_attr1(usize addr);
#define page_get_attr(...) CONCAT(page_get_attr, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

usize page_get_phy2(usize addr, usize pd);
usize page_get_phy1(usize addr);
#define page_get_phy(...) CONCAT(page_get_phy, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

void tpo2page(int *page, int t, int p);

void *page_malloc_one_count_from_4gb();

void *page_alloc(size_t size);

void page_free(void *p, size_t size);

usize pd_clone(usize addr);

void pd_free(usize addr);

void *page_malloc_one();

void page_link2(usize addr, usize pd);
void page_link1(usize addr);
#define page_link(...) CONCAT(page_link, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

void page_link_share2(usize addr, usize pd);
void page_link_share1(usize addr);
#define page_link_share(...) CONCAT(page_link_share, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

void page_unlink(u32 addr);

u32 page_get_alloced();

/**
 *\brief 
 *
 *\param addr     目标(虚拟)地址
 *\param cr3      分页设置(cr3)
 *\param map_addr 源(物理)地址
 */
void page_link_addr_pde(usize addr, usize pd, usize map_addr);

// u8 PageType
enum {
  PAGE_TYPE_CODE,  // 代码页
  PAGE_TYPE_DATA,  // 数据页
  PAGE_TYPE_STACK, // 栈页
  PAGE_TYPE_HEAP,  // 堆页
  PAGE_TYPE_ALLOC, // 分配页
};

// --------------------------------------------------
//; 内存权限检查

/**
 *\brief 检查用户是否有对指定内存地址的访问权限
 *
 *\param addr     内存地址
 *\param wr       是否写权限     (默认 false)
 *\param cr3      分页设置(cr3)  (默认 当前 cr3)
 *\return 是否有权限
 */
bool check_address_permission3(const void *addr, bool wr, usize cr3);

/**
 *\brief 检查用户是否有对指定内存地址的访问权限
 *
 *\param addr     内存地址
 *\param wr       是否写权限    (默认 false)
 *\return 是否有权限
 */
bool check_address_permission2(const void *addr, bool wr);

/**
 *\brief 检查用户是否有对指定内存地址的访问权限
 *
 *\param addr     内存地址
 *\return 是否有权限
 */
bool check_address_permission1(const void *addr);

#define check_address_permission(...)                                                              \
  CONCAT(check_address_permission, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

/**
 *\brief 检查用户是否有对指定内存区域的访问权限
 *
 *\param addr     内存地址
 *\param size     内存大小
 *\param wr       是否写权限
 *\param cr3      分页设置(cr3)  (默认 当前 cr3)
 *\return 是否有权限
 */
bool check_memory_permission4(const void *addr, size_t size, bool wr, usize cr3);

/**
 *\brief 检查用户是否有对指定内存区域的访问权限
 *
 *\param addr     内存地址
 *\param size     内存大小
 *\param wr       是否写权限
 *\return 是否有权限
 */
bool check_memory_permission3(const void *addr, size_t size, bool wr);

/**
 *\brief 检查用户是否有对指定内存区域的访问权限
 *
 *\param addr     内存地址
 *\param size     内存大小
 *\return 是否有权限
 */
bool check_memory_permission2(const void *addr, size_t size);

#define check_memory_permission(...)                                                               \
  CONCAT(check_memory_permission, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

/**
 *\brief 检查用户是否有对指定字符串的读取权限
 *
 *\param addr     内存地址
 *\param cr3      分页设置(cr3)  (默认 当前 cr3)
 *\return 是否有权限
 */
bool check_string_permission2(cstr addr, usize cr3);

/**
 *\brief 检查用户是否有对指定字符串的读取权限
 *
 *\param addr     内存地址
 *\return 是否有权限
 */
bool check_string_permission1(cstr addr);

#define check_string_permission(...)                                                               \
  CONCAT(check_string_permission, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)
