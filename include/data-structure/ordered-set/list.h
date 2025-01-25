#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define LIST_IMPLEMENTATION
#endif

/**
 *\struct ListNode
 *\brief 链表节点结构
 */
typedef struct list *list_t;
struct list {
  union {
    void *data;
    isize idata;
    usize udata;
  };
  list_t prev;
  list_t next;
};

#ifdef LIST_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的链表节点
 *\param[in] data 节点数据
 *\return 新的链表节点指针
 */
extern list_t list_alloc(void *data);

/**
 *\brief 删除整个链表
 *\param[in] list 链表头指针
 *\return 恒为 null
 */
extern list_t list_free(list_t list);

/**
 *\brief 删除整个链表
 *\param[in] list 链表头指针
 *\param[in] free_data 释放数据的 callback
 *\return 恒为 null
 */
extern list_t list_free_with(list_t list, free_t free_data);

/**
 *\brief 在链表末尾插入节点
 *\param[in] list 链表头指针
 *\param[in] data 节点数据
 *\return 更新后的链表头指针
 */
extern list_t list_append(list_t list, void *data);

/**
 *\brief 获取链表头
 *\param[in] list 链表指针
 *\return 存在则为指向链表头的指针，否则为 null
 */
extern list_t list_head(list_t list);

/**
 *\brief 获取链表尾
 *\param[in] list 链表指针
 *\return 存在则为指向链表尾的指针，否则为 null
 */
extern list_t list_tail(list_t list);

/**
 *\brief 获取链表的第 n 项
 *\param[in] list 链表指针 (最好是头指针)
 *\param[in] n 序号 (从 0 开始)
 *\return 存在则为指向该项的指针，否则为 null
 */
extern list_t list_nth(list_t list, size_t n);

/**
 *\brief 获取链表的倒数第 n 项
 *\param[in] list 链表指针 (最好是尾指针)
 *\param[in] n 序号 (从 0 开始倒数)
 *\return 存在则为指向该项的指针，否则为 null
 */
extern list_t list_nth_last(list_t list, size_t n);

/**
 *\brief 在链表开头插入节点
 *\param[in] list 链表头指针
 *\param[in] data 节点数据
 *\return 更新后的链表头指针
 */
extern list_t list_prepend(list_t list, void *data);

/**
 *\brief 在链表中查找节点
 *\param[in] list 链表头指针
 *\param[in] data 要查找的节点数据
 *\return 若找到对应节点，则返回true，否则返回false
 */
extern bool list_search(list_t list, void *data);

/**
 *\brief 删除链表中的节点
 *\param[in] list 链表头指针
 *\param[in] data 要删除的节点数据
 *\return 更新后的链表头指针
 */
extern list_t list_delete(list_t list, void *data);

/**
 *\brief 删除链表中的节点
 *\param[in] slist 链表头指针
 *\param[in] node 要删除的节点
 *\return 更新后的链表头指针
 */
extern list_t list_delete_node(list_t list, list_t node);

/**
 *\brief 链表的长度
 *\param[in] list 链表头指针
 *\return 链表的长度
 */
extern size_t list_length(list_t list);

/**
 *\brief 打印链表中的节点数据
 *\param[in] list 链表头指针
 */
extern void list_print(list_t list);

#ifdef LIST_IMPLEMENTATION
#  undef extern
#endif

#ifdef LIST_IMPLEMENTATION

static list_t list_alloc(void *data) {
  list_t node = malloc(sizeof(*node));
  if (node == null) return null;
  node->data = data;
  node->prev = null;
  node->next = null;
  return node;
}

static list_t list_free(list_t list) {
  while (list != null) {
    list_t next = list->next;
    free(list);
    list = next;
  }
  return null;
}

static list_t list_free_with(list_t list, void (*free_data)(void *)) {
  while (list != null) {
    list_t next = list->next;
    free_data(list->data);
    free(list);
    list = next;
  }
  return null;
}

static list_t list_append(list_t list, void *data) {
  list_t node = list_alloc(data);
  if (node == null) return list;

  if (list == null) {
    list = node;
  } else {
    list_t current = list;
    while (current->next != null) {
      current = current->next;
    }
    current->next = node;
    node->prev    = current;
  }

  return list;
}

static list_t list_prepend(list_t list, void *data) {
  list_t node = list_alloc(data);
  if (node == null) return list;

  node->next = list;
  if (list != null) list->prev = node;
  list = node;

  return list;
}

static void *list_pop(list_t *list_p) {
  if (list_p == null || *list_p == null) return null;
  auto list = list_tail(*list_p);
  if (*list_p == list) *list_p = list->prev;
  if (list->prev) list->prev->next = null;
  auto data = list->data;
  free(list);
  return data;
}

static list_t list_head(list_t list) {
  if (list == null) return null;
  for (; list->prev; list = list->prev) {}
  return list;
}

static list_t list_tail(list_t list) {
  if (list == null) return null;
  for (; list->next; list = list->next) {}
  return list;
}

static list_t list_nth(list_t list, size_t n) {
  if (list == null) return null;
  list = list_head(list);
  for (size_t i = 0; i < n; i++) {
    list = list->next;
    if (list == null) return null;
  }
  return list;
}

static list_t list_nth_last(list_t list, size_t n) {
  if (list == null) return null;
  list = list_tail(list);
  for (size_t i = 0; i < n; i++) {
    list = list->prev;
    if (list == null) return null;
  }
  return list;
}

static bool list_search(list_t list, void *data) {
  list_t current = list;
  while (current != null) {
    if (current->data == data) return true;
    current = current->next;
  }
  return false;
}

static list_t list_delete(list_t list, void *data) {
  if (list == null) return null;

  if (list->data == data) {
    list_t temp = list;
    list        = list->next;
    free(temp);
    return list;
  }

  for (list_t current = list->next; current; current = current->next) {
    if (current->data == data) {
      current->prev->next = current->next;
      if (current->next != null) current->next->prev = current->prev;
      free(current);
      break;
    }
  }

  return list;
}

static list_t list_delete_with(list_t list, void *data, free_t callback) {
  if (list == null) return null;

  if (list->data == data) {
    list_t temp = list;
    list        = list->next;
    if (callback) callback(temp->data);
    free(temp);
    return list;
  }

  for (list_t current = list->next; current; current = current->next) {
    if (current->data == data) {
      current->prev->next = current->next;
      if (current->next != null) current->next->prev = current->prev;
      if (callback) callback(current->data);
      free(current);
      break;
    }
  }

  return list;
}

static list_t list_delete_node(list_t list, list_t node) {
  if (list == null || node == null) return list;

  if (list == node) {
    list_t temp = list;
    list        = list->next;
    free(temp);
    return list;
  }

  node->prev->next = node->next;
  if (node->next != null) node->next->prev = node->prev;
  free(node);
  return list;
}

static list_t list_delete_node_with(list_t list, list_t node, free_t callback) {
  if (list == null || node == null) return list;

  if (list == node) {
    list_t temp = list;
    list        = list->next;
    if (callback) callback(temp->data);
    free(temp);
    return list;
  }

  node->prev->next = node->next;
  if (node->next != null) node->next->prev = node->prev;
  if (callback) callback(node->data);
  free(node);
  return list;
}

static size_t list_length(list_t list) {
  size_t count   = 0;
  list_t current = list;
  while (current != null) {
    count++;
    current = current->next;
  }
  return count;
}

static void list_print(list_t list) {
  list_t current = list;
  while (current != null) {
    printf("%p -> ", current->data);
    current = current->next;
  }
  printf("null\n");
}

#  undef LIST_IMPLEMENTATION
#endif

/**
 *\brief 在链表末尾插入节点
 *\param[in,out] list 链表头指针
 *\param[in] data 节点数据
 */
#define list_append(list, data) ((list) = list_append(list, (void *)(data)))

/**
 *\brief 在链表开头插入节点
 *\param[in,out] list 链表头指针
 *\param[in] data 节点数据
 */
#define list_prepend(list, data) ((list) = list_prepend(list, (void *)(data)))

#define list_push(list, data) list_append(list, data)

#define list_pop(list) list_pop(&(list))

#define list_popi(list) ((usize)list_pop(list))
#define list_popu(list) ((isize)list_pop(list))

/**
 *\brief 删除链表中的节点
 *\param[in,out] list 链表头指针
 *\param[in] data 要删除的节点数据
 */
#define list_delete(list, data) ((list) = list_delete(list, data))

#define list_delete_with(list, data, callback) ((list) = list_delete_with(list, data, callback))

/**
 *\brief 删除链表中的节点
 *\param[in,out] list 链表头指针
 *\param[in] node 要删除的节点
 */
#define list_delete_node(slist, node) ((slist) = list_delete_node(slist, node))

#define list_delete_node_with(list, node, callback)                                                \
  ((list) = list_delete_node_with(list, node, callback))

/**
 *\brief 遍历链表中的节点并执行操作
 *\param[in] list 链表头指针
 *\param[in] node 用于迭代的节点指针变量
 */
#define list_foreach(list, node) for (list_t node = (list); node; node = node->next)

/**
 *\brief 遍历链表中的节点并执行操作
 *\param[in] list 链表头指针
 *\param[in] node 用于迭代的节点指针变量
 */
#define list_foreach_cnt(list, i, node, ...)                                                       \
  ({                                                                                               \
    size_t i = 0;                                                                                  \
    for (list_t node = (list); node; (node) = (node)->next, (i)++) {                               \
      (__VA_ARGS__);                                                                               \
    }                                                                                              \
  })

#define list_first_node(list, node, expr)                                                          \
  ({                                                                                               \
    list_t _match_ = null;                                                                         \
    for (list_t node = (list); node; node = node->next) {                                          \
      if ((expr)) {                                                                                \
        _match_ = node;                                                                            \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    _match_;                                                                                       \
  })

#define list_first(list, _data_, expr)                                                             \
  ({                                                                                               \
    void *_match_ = null;                                                                          \
    for (list_t node = (list); node; node = node->next) {                                          \
      void *_data_ = node->data;                                                                   \
      if ((expr)) {                                                                                \
        _match_ = _data_;                                                                          \
        break;                                                                                     \
      }                                                                                            \
    }                                                                                              \
    _match_;                                                                                       \
  })
