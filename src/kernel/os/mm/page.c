#include <kernel.h>

// 目前没用

// 1024 x 1024 x 4096

// PAGE_MANNAGER 物理页面管理器可使用的内存开始处

#define KERNEL_STATIC_MEM_END ((size_t)0xb00000)

typedef struct pages_info *pages_info_t;
struct pages_info {
  size_t       size; // 页面数
  size_t       rc;   // 引用计数
  pages_info_t next;
  pages_info_t prev;
};

static pages_info_t       pages_ptr[1024];
static size_t             pages_size[1024]; // 页表中剩余可分配大小 (页面数)
static struct sized_mpool pool;

finline pages_info_t info_alloc() {
  pages_info_t ptr = sized_mpool_alloc(&pool);
  ptr->size        = 0;
  ptr->rc          = 0;
  ptr->next        = null;
  ptr->prev        = null;
  return ptr;
}
finline void info_free(pages_info_t ptr) {
  sized_mpool_free(&pool, ptr);
}

/**
 *\brief 找到第一个页面数大于指定数的连续空闲页
 *
 * 会对空闲内存进行切分
 *
 *\param size     页面数
 *\return 找到的页面起始地址，未找到为 NULL
 */
finline void *find_page(size_t size) {
  for (size_t i = 0; i < 1024; i++) {
    pages_info_t info = pages_ptr[i];
    if (info == null) return null;
    for (; info != null; info = info->next) {
      if (info->size >= size) { return info; }
    }
  }
  return 0;
}

/**
 *\brief 初始化物理内存管理器
 *
 *\param base     param
 *\param size     物理内存大小
 */
void physical_mman_init(void *base, size_t size) {
  size_t mman_size    = KERNEL_STATIC_MEM_END - PAGE_MANNAGER;
  size_t mman_blksize = sizeof(struct pages_info);
  sized_mpool_init(&pool, (void *)PAGE_MANNAGER, mman_blksize, mman_size / mman_blksize);

  size_t npages = size & ~(PAGE_SIZE * 1024 - 1); // n 个完整页表目录
  for (size_t i = 0; i < npages; i++) {
    auto ptr      = info_alloc();
    ptr->size     = 1024;
    pages_ptr[i]  = ptr;
    pages_size[i] = 1024;
  }
}

void physical_mman_alloc(size_t size) {}

void physical_mman_free(void *ptr, size_t size) {}

/**
 *\brief 引用某块物理内存，使引用计数加一
 *
 *\param ptr      param
 *\param size     param
 */
void physical_mman_ref(void *ptr, size_t size) {
  if ((size_t)ptr & (PAGE_SIZE - 1)) fatal("传入参数未对齐 ptr: %p", ptr);
  if (size & (PAGE_SIZE - 1)) fatal("传入参数未对齐 size: %p", size);
}

void physical_mman_unref(void *ptr, size_t size) {
  if ((size_t)ptr & (PAGE_SIZE - 1)) fatal("传入参数未对齐 ptr: %p", ptr);
  if (size & (PAGE_SIZE - 1)) fatal("传入参数未对齐 size: %p", size);
}

size_t physical_mman_rc(void *ptr) {
  if ((size_t)ptr & (PAGE_SIZE - 1)) fatal("传入参数未对齐 ptr: %p", ptr);
  return 0;
}
