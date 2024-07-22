#pragma once
#include "../stdio/print.h"
#include "../stdlib/alloc.h"
#define LIST_IMPLEMENTATION

/**
 *\struct ListNode
 *\brief 链表节点结构
 */
typedef struct list *list_t;
struct list {
  void  *data;
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
 */
extern void list_free(list_t list);

extern void list_free_with(list_t list, void (*free_data)(void *));

/**
 *\brief 在链表末尾插入节点
 *\param[in] list 链表头指针
 *\param[in] data 节点数据
 *\return 更新后的链表头指针
 */
extern list_t list_append(list_t list, void *data);

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

static void list_free(list_t list) {
  while (list != null) {
    list_t next = list->next;
    free(list);
    list = next;
  }
}

static void list_free_with(list_t list, void (*free_data)(void *)) {
  while (list != null) {
    list_t next = list->next;
    free_data(list->data);
    free(list);
    list = next;
  }
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

static list_t list_delete_node(list_t list, list_t node) {
  if (list == null || node == null) return list;

  if (list == node) {
    list_t temp = list;
    list        = list->next;
    free(temp);
    return list;
  }

  for (list_t current = list->next; current; current = current->next) {
    if (current == node) {
      current->prev->next = current->next;
      if (current->next != null) current->next->prev = current->prev;
      free(current);
      break;
    }
  }

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
#define list_append(list, data) ((list) = list_append(list, data))

/**
 *\brief 在链表开头插入节点
 *\param[in,out] list 链表头指针
 *\param[in] data 节点数据
 */
#define list_prepend(list, data) ((list) = list_prepend(list, data))

/**
 *\brief 删除链表中的节点
 *\param[in,out] list 链表头指针
 *\param[in] data 要删除的节点数据
 */
#define list_delete(list, data) ((list) = list_delete(list, data))

/**
 *\brief 删除链表中的节点
 *\param[in,out] list 链表头指针
 *\param[in] node 要删除的节点
 */
#define list_delete_node(slist, node) ((slist) = list_delete_node(slist, node))

/**
 *\brief 遍历链表中的节点并执行操作
 *\param[in] list 链表头指针
 *\param[in] node 用于迭代的节点指针变量
 */
#define list_foreach(list, node) for (list_t node = (list); node; node = node->next)
