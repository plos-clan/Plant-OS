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
#define PAGE_BIT9    MASK(9)  // 可自定义位 (bit9)
#define PAGE_BIT10   MASK(10) // 可自定义位 (bit9)
#define PAGE_BIT11   MASK(11) // 可自定义位 (bit9)
#define PAGE_SHARED  MASK(10) // 自定义的
#define PAGE_NX      MASK(63) // 不可执行

#define PD_ADDRESS    0x400000
#define PT_ADDRESS    (PD_ADDRESS + 0x1000)
#define PAGE_END      (PT_ADDRESS + 0x400000)
#define PAGE_MANNAGER PAGE_END

#define PF_PRESENT  MASK(0) // 程序尝试访问的页面是否存在于内存中
#define PF_WRITE    MASK(1) // 程序进行的访问类型是否为写入
#define PF_USER     MASK(2) // 错误是否在用户态下发生
#define PF_RESERVED MASK(3) // 程序是否尝试访问保留的地址
#define PF_IFETCH   MASK(4) // 是否为 cpu 取指时发生的错误

// 注意 user_wr 位
//     当页面是 cow 页且有多个程序引用时，其 wrable 应为 false
//     但 user_wr 为 true，以便在触发 pf 时将 cow 页复制
// 注意 dirty 标识只应在最后一级页表中使用
//     其余级别中虽然可以被操作系统用于存储其它数据但在 plos 中不应该使用
// 注意 ps 标识在高级别的页表上是保留的

#ifdef __x86_64__

typedef struct PML5E {
  usize present  : 1;  // 存在位
  usize wrable   : 1;  // 读写位
  usize user     : 1;  // 用户位
  usize wt       : 1;  // 直写位
  usize cd       : 1;  // 禁用缓存
  usize access   : 1;  // 访问位
  usize _dirty   : 1;  // 不应该使用 (在其它级别的页表上标识脏页)
  usize _ps      : 1;  // 禁止使用 (在其它级别的页表上标识页大小)
  usize global   : 1;  // 全局页
  usize user_wr  : 1;  // 用户是否可写 (自定义的)
  usize shared   : 1;  // 共享页 (自定义的)
  usize bit11    : 1;  //
  usize addr     : 40; // 一定要左移 12 位
  usize reserved : 11; // 保留位
  usize nx       : 1;  // 不可执行
} PML5E;

typedef struct PML4E {
  usize present  : 1;  // 存在位
  usize wrable   : 1;  // 读写位
  usize user     : 1;  // 用户位
  usize wt       : 1;  // 直写位
  usize cd       : 1;  // 禁用缓存
  usize access   : 1;  // 访问位
  usize _dirty   : 1;  // 不应该使用 (在其它级别的页表上标识脏页)
  usize _ps      : 1;  // 禁止使用 (在其它级别的页表上标识页大小)
  usize global   : 1;  // 全局页
  usize user_wr  : 1;  // 用户是否可写 (自定义的)
  usize shared   : 1;  // 共享页 (自定义的)
  usize bit11    : 1;  //
  usize addr     : 40; // 一定要左移 12 位
  usize reserved : 11; // 保留位
  usize nx       : 1;  // 不可执行
} PML4E;

typedef struct PDPTE {
  usize present  : 1;  // 存在位
  usize wrable   : 1;  // 读写位
  usize user     : 1;  // 用户位
  usize wt       : 1;  // 直写位
  usize cd       : 1;  // 禁用缓存
  usize access   : 1;  // 访问位
  usize dirty    : 1;  // 脏页
  usize ps       : 1;  // 页大小
  usize global   : 1;  // 全局页
  usize user_wr  : 1;  // 用户是否可写 (自定义的)
  usize shared   : 1;  // 共享页 (自定义的)
  usize bit11    : 1;  //
  usize addr     : 40; // 一定要左移 12 位
  usize reserved : 11; // 保留位
  usize nx       : 1;  // 不可执行
} PDPTE;

#endif

typedef struct PDE {
  usize present : 1; // 存在位
  usize wrable  : 1; // 读写位
  usize user    : 1; // 用户位
  usize wt      : 1; // 直写位
  usize cd      : 1; // 禁用缓存
  usize access  : 1; // 访问位
  usize dirty   : 1; // 脏页
  usize ps      : 1; // 页大小
  usize global  : 1; // 全局页
  usize user_wr : 1; // 用户是否可写 (自定义的)
  usize shared  : 1; // 共享页 (自定义的)
  usize bit11   : 1; //
#ifndef __x86_64__
  usize addr : 20; // 一定要左移 12 位
#else
  usize addr     : 40; // 一定要左移 12 位
  usize reserved : 11; // 保留位
  usize nx       : 1;  // 不可执行
#endif
} PDE;

typedef struct PTE {
  usize present : 1; // 存在位
  usize wrable  : 1; // 读写位
  usize user    : 1; // 用户位
  usize wt      : 1; // 直写位
  usize cd      : 1; // 禁用缓存
  usize access  : 1; // 访问位
  usize dirty   : 1; // 脏页
  usize pat     : 1; // 页属性表
  usize global  : 1; // 全局页
  usize user_wr : 1; // 用户是否可写 (自定义的)
  usize shared  : 1; // 共享页 (自定义的)
  usize bit11   : 1; //
#ifndef __x86_64__
  usize addr : 20; // 一定要左移 12 位
#else
  usize addr     : 40; // 一定要左移 12 位
  usize reserved : 11; // 保留位
  usize nx       : 1;  // 不可执行
#endif
} PTE;

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
inline_const usize mk_linear_addr5(usize pml4, usize pdpt, usize pd, usize pt, usize off) {
  return (pml4 << 39) + (pdpt << 30) + (pd << 21) + (pt << 12) + off;
}
inline_const usize mk_linear_addr4(usize pml4, usize pdpt, usize pd, usize pt) {
  return (pml4 << 39) + (pdpt << 30) + (pd << 21) + (pt << 12);
}
inline_const usize mk_linear_addr3(usize pml4, usize pdpt, usize pd) {
  return (pml4 << 39) + (pdpt << 30) + (pd << 21);
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
inline_const usize mk_linear_addr3(usize table, usize page, usize off) {
  kassert(table < 1024);
  kassert(page < 1024);
  kassert(off < 4096);
  return (table << 22) + (page << 12) + off;
}
/**
 *\brief 构建线性地址 (10 + 10 + 12)
 *
 *\param table    页目录地址
 *\param page     页表地址
 *\return 线性地址
 */
inline_const usize mk_linear_addr2(usize table, usize page) {
  kassert(table < 1024);
  kassert(page < 1024);
  return (table << 22) + (page << 12);
}
#endif
#define mk_linear_addr(...) CONCAT(mk_linear_addr, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

finline PTE *getlinearpte2(usize t, usize p) {
  usize addr = mk_linear_addr(t, p);
  return (PTE *)(PT_ADDRESS + (addr >> 12) * 4);
}
finline PTE *getlinearpte1(usize addr) {
  return (PTE *)(PT_ADDRESS + (addr >> 12) * 4);
}
#define getlinearpte(...) CONCAT(getlinearpte, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

usize page_get_attr2(usize addr, usize pd);
usize page_get_attr1(usize addr);
#define page_get_attr(...) CONCAT(page_get_attr, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

usize page_get_phy2(usize addr, usize pd);
usize page_get_phy1(usize addr);
#define page_get_phy(...) CONCAT(page_get_phy, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

void *page_malloc_one_count_from_4gb();

void *page_alloc(usize size);

void page_free(void *p, usize size);

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
bool check_memory_permission4(const void *addr, usize size, bool wr, usize cr3);

/**
 *\brief 检查用户是否有对指定内存区域的访问权限
 *
 *\param addr     内存地址
 *\param size     内存大小
 *\param wr       是否写权限
 *\return 是否有权限
 */
bool check_memory_permission3(const void *addr, usize size, bool wr);

/**
 *\brief 检查用户是否有对指定内存区域的访问权限
 *
 *\param addr     内存地址
 *\param size     内存大小
 *\return 是否有权限
 */
bool check_memory_permission2(const void *addr, usize size);

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
