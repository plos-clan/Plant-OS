#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define SLIST_IMPLEMENTATION
#endif

/**
 *\struct ListNode
 *\brief 单向链表节点结构
 */
typedef struct slist *slist_t;
struct slist {
  void   *data;
  slist_t next;
};

#ifdef SLIST_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的单向链表节点
 *\param[in] data 节点数据
 *\return 新的单向链表节点指针
 */
extern slist_t slist_alloc(void *data);

/**
 *\brief 删除整个单向链表
 *\param[in] slist 单向链表头指针
 */
extern void slist_free(slist_t slist);

/**
 *\brief 在单向链表末尾插入节点
 *\param[in] slist 单向链表头指针
 *\param[in] data 节点数据
 *\return 更新后的单向链表头指针
 */
extern slist_t slist_append(slist_t slist, void *data);

/**
 *\brief 在单向链表开头插入节点
 *\param[in] slist 单向链表头指针
 *\param[in] data 节点数据
 *\return 更新后的单向链表头指针
 */
extern slist_t slist_prepend(slist_t slist, void *data);

/**
 *\brief 在单向链表中查找节点
 *\param[in] slist 单向链表头指针
 *\param[in] data 要查找的节点数据
 *\return 若找到对应节点，则返回true，否则返回false
 */
extern bool slist_search(slist_t slist, void *data);

/**
 *\brief 删除单向链表中的节点
 *\param[in] slist 单向链表头指针
 *\param[in] data 要删除的节点数据
 *\return 更新后的单向链表头指针
 */
extern slist_t slist_delete(slist_t slist, void *data);

/**
 *\brief 删除单向链表中的节点
 *\param[in] slist 单向链表头指针
 *\param[in] node 要删除的节点
 *\return 更新后的单向链表头指针
 */
extern slist_t slist_delete_node(slist_t slist, slist_t node);

/**
 *\brief 单向链表的长度
 *\param[in] slist 单向链表头指针
 *\return 单向链表的长度
 */
extern size_t slist_length(slist_t slist);

/**
 *\brief 打印单向链表中的节点数据
 *\param[in] slist 单向链表头指针
 */
extern void slist_print(slist_t slist);

#ifdef SLIST_IMPLEMENTATION
#  undef extern
#endif

#ifdef SLIST_IMPLEMENTATION

static slist_t slist_alloc(void *data) {
  slist_t node = malloc(sizeof(*node));
  if (node == null) return null;
  node->data = data;
  node->next = null;
  return node;
}

static void slist_free(slist_t slist) {
  while (slist != null) {
    slist_t next = slist->next;
    free(slist);
    slist = next;
  }
}

static slist_t slist_append(slist_t slist, void *data) {
  slist_t node = slist_alloc(data);
  if (node == null) return slist;

  if (slist == null) {
    slist = node;
  } else {
    slist_t current = slist;
    while (current->next != null) {
      current = current->next;
    }
    current->next = node;
  }

  return slist;
}

static slist_t slist_prepend(slist_t slist, void *data) {
  slist_t node = slist_alloc(data);
  if (node == null) return slist;

  node->next = slist;
  slist      = node;

  return slist;
}

static bool slist_search(slist_t slist, void *data) {
  slist_t current = slist;
  while (current != null) {
    if (current->data == data) return true;
    current = current->next;
  }
  return false;
}

static slist_t slist_delete(slist_t slist, void *data) {
  if (slist == null) return null;

  if (slist->data == data) {
    slist_t temp = slist;
    slist        = slist->next;
    free(temp);
    return slist;
  }

  slist_t prev = slist;
  for (slist_t current = slist->next; current != null; current = current->next) {
    if (current->data == data) {
      prev->next = current->next;
      free(current);
      break;
    }
    prev = current;
  }

  return slist;
}

static slist_t slist_delete_node(slist_t slist, slist_t node) {
  if (slist == null || node == null) return slist;

  if (slist == node) {
    slist_t temp = slist;
    slist        = slist->next;
    free(temp);
    return slist;
  }

  for (slist_t current = slist; current->next; current = current->next) {
    if (current->next == node) {
      current->next = node->next;
      free(node);
      break;
    }
  }

  return slist;
}

static size_t slist_length(slist_t slist) {
  size_t  count   = 0;
  slist_t current = slist;
  while (current != null) {
    count++;
    current = current->next;
  }
  return count;
}

static void slist_print(slist_t slist) {
  slist_t current = slist;
  while (current != null) {
    printf("%p -> ", current->data);
    current = current->next;
  }
  printf("null\n");
}

#  undef SLIST_IMPLEMENTATION
#endif

/**
 *\brief 在单向链表末尾插入节点
 *\param[in,out] slist 单向链表头指针
 *\param[in] data 节点数据
 */
#define slist_append(slist, data) ((slist) = slist_append(slist, data))

/**
 *\brief 在单向链表开头插入节点
 *\param[in,out] slist 单向链表头指针
 *\param[in] data 节点数据
 */
#define slist_prepend(slist, data) ((slist) = slist_prepend(slist, data))

/**
 *\brief 删除单向链表中的节点
 *\param[in,out] slist 单向链表头指针
 *\param[in] data 要删除的节点数据
 */
#define slist_delete(slist, data) ((slist) = slist_delete(slist, data))

/**
 *\brief 删除单向链表中的节点
 *\param[in] slist 单向链表头指针
 *\param[in] node 要删除的节点
 */
#define slist_delete_node(slist, node) ((slist) = slist_delete_node(slist, node))

/**
 *\brief 遍历单向链表中的节点并执行操作
 *\param[in] slist 单向链表头指针
 *\param[in] node 用于迭代的节点指针变量
 */
#define slist_foreach(slist, node) for (slist_t node = (slist); node; node = node->next)
