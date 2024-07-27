#pragma once
#include <libc.h>

#include "block.h"

#define FREELISTS_NUM 8

#define FREELIST_MAXBLKSIZE 16384

typedef struct freelist *freelist_t;
struct freelist {
  freelist_t next;
  freelist_t prev;
};

typedef freelist_t freelists_t[FREELISTS_NUM];

//; 获取该大小属于freelists中的哪个list
finline int freelists_size2id(size_t size) {
  if (size < 64) return 0;
  if (size < 256) return 1;
  if (size >= 16384) return -1;
  return (32 - 9) - clz((u32)size) + 2;
}

/**
 *\brief 将 freelist 中的内存块分离
 *
 *\param lists    param
 *\param id       param
 *\param ptr      param
 *\return value
 */
finline void *freelist_detach(freelists_t lists, int id, freelist_t ptr) {
  if (lists[id] == ptr) {
    lists[id] = ptr->next;
    return ptr;
  }

  if (ptr->next) ptr->next->prev = ptr->prev;
  if (ptr->prev) ptr->prev->next = ptr->next;
  return ptr;
}

/**
 *\brief 匹配并将内存从 freelist 中分离
 *
 *\param lists    param
 *\param size     param
 *\return value
 */
finline void *freelists_match(freelists_t lists, size_t size) {
  int id = freelists_size2id(size);
  if (id < 0) return null;
  for (; id < FREELISTS_NUM; id++) {
    for (freelist_t list = lists[id]; list != null; list = list->next) {
      size_t tgt_size = blk_size(list);
      if (tgt_size >= size) return freelist_detach(lists, id, list);
    }
  }
  return null;
}

/**
 *\brief 将元素放到 freelist 中
 *
 *\param lists    param
 *\param ptr      param
 *\return value
 */
finline bool freelists_put(freelists_t lists, void *_ptr) {
  freelist_t ptr  = _ptr;
  size_t     size = blk_size(ptr);
  int        id   = freelists_size2id(size);
  if (id < 0) return false;
  ptr->next = lists[id];
  ptr->prev = null;
  lists[id] = ptr;
  if (ptr->next) ptr->next->prev = ptr;
  return true;
}
