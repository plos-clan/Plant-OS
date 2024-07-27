#pragma once
#include <define.h>
#include <type.h>

#ifdef ALL_IMPLEMENTATION
#  define ARRAYLIST_IMPLEMENTATION
#endif

/**
 *\struct Array List Node
 *\brief 数组模拟的链表节点结构
 */
typedef struct arraylist_node *arraylist_node_t;
struct arraylist_node {
  void            *data;
  arraylist_node_t next;
  arraylist_node_t prev;
};

/**
 *\struct Array List
 *\brief 数组模拟的链表结构
 */
typedef struct arraylist {
  arraylist_node_t arr;
  arraylist_node_t head;
  arraylist_node_t tail;
  arraylist_node_t freed;
} *arraylist_t;

#ifdef ARRAYLIST_IMPLEMENTATION
#  define extern static
#endif

extern void arraylist_init(arraylist_t list, arraylist_node_t arr, size_t len);

extern void arraylist_clear(arraylist_t list);

extern void arraylist_clear_with(arraylist_t list, void (*callback)(void *));

extern bool arraylist_append(arraylist_t list, void *data);

extern bool arraylist_prepend(arraylist_t list, void *data);

extern arraylist_node_t arraylist_nth(arraylist_t list, size_t n);

extern arraylist_node_t arraylist_nth_last(arraylist_t list, size_t n);

extern bool arraylist_search(arraylist_t list, void *data);

extern void arraylist_delete(arraylist_t list, void *data);

extern void arraylist_delete_node(arraylist_t list, arraylist_t node);

extern size_t arraylist_length(arraylist_t list);

#ifdef ARRAYLIST_IMPLEMENTATION
#  undef extern
#endif

#ifdef ARRAYLIST_IMPLEMENTATION

static void arraylist_init(arraylist_t list, arraylist_node_t arr, size_t len) {
  if (list == null) return;
  list->arr   = arr;
  list->head  = null;
  list->tail  = null;
  list->freed = arr;
  for (size_t i = 1; i < len; i++) {
    arr[i - 1].next = &arr[i];
  }
  arr[len - 1].next = null;
}

static void arraylist_clear(arraylist_t list) {
  if (list == null) return;
  arraylist_node_t next;
  for (auto node = list->head; node != null; node = next) {
    next        = node->next;
    node->next  = list->freed;
    list->freed = node;
  }
}

static void arraylist_clear_with(arraylist_t list, void (*callback)(void *)) {
  if (list == null) return;
  arraylist_node_t next;
  for (auto node = list->head; node != null; node = next) {
    next = node->next;
    if (callback) callback(node->data);
    node->next  = list->freed;
    list->freed = node;
  }
}

static bool arraylist_append(arraylist_t list, void *data) {
  if (list == null) return false;
  if (list->freed == null) return false;
  auto node   = list->freed;
  list->freed = list->freed->next;
  node->data  = data;
  node->prev  = list->tail;
  node->next  = null;
  if (list->tail != null) list->tail->next = node;
  if (list->head == null) list->head = node;
  list->tail = node;
  return true;
}

static bool arraylist_prepend(arraylist_t list, void *data) {
  if (list == null) return false;
  if (list->freed == null) return false;
  auto node   = list->freed;
  list->freed = list->freed->next;
  node->data  = data;
  node->prev  = null;
  node->next  = list->head;
  if (list->head != null) list->head->prev = node;
  if (list->tail == null) list->tail = node;
  list->head = node;
  return true;
}

static arraylist_node_t arraylist_nth(arraylist_t list, size_t n) {
  if (list == null || list->head == null) return null;
  auto node = list->head;
  for (size_t i = 0; i < n; i++) {
    node = node->next;
    if (node == null) return null;
  }
  return node;
}

static arraylist_node_t arraylist_nth_last(arraylist_t list, size_t n) {
  if (list == null || list->tail == null) return null;
  auto node = list->tail;
  for (size_t i = 0; i < n; i++) {
    node = node->prev;
    if (node == null) return null;
  }
  return node;
}

static bool arraylist_search(arraylist_t list, void *data) {
  auto current = list->head;
  while (current != null) {
    if (current->data == data) return true;
    current = current->next;
  }
  return false;
}

// static void arraylist_delete(arraylist_t list, void *data) {
//   if (list == null) return null;

//   if (list->data == data) {
//     arraylist_t temp = list;
//     list             = list->next;
//     free(temp);
//     return list;
//   }

//   for (arraylist_t current = list->next; current; current = current->next) {
//     if (current->data == data) {
//       current->prev->next = current->next;
//       if (current->next != null) current->next->prev = current->prev;
//       free(current);
//       break;
//     }
//   }

//   return list;
// }

// static arraylist_t arraylist_delete_node(arraylist_t list, arraylist_t node) {
//   if (list == null || node == null) return list;

//   if (list == node) {
//     arraylist_t temp = list;
//     list             = list->next;
//     free(temp);
//     return list;
//   }

//   node->prev->next = node->next;
//   if (node->next != null) node->next->prev = node->prev;
//   free(node);
//   return list;
// }

static size_t arraylist_length(arraylist_t list) {
  size_t count   = 0;
  auto   current = list->head;
  while (current != null) {
    count++;
    current = current->next;
  }
  return count;
}

#  undef ARRAYLIST_IMPLEMENTATION
#endif

#define arraylist_append(list, data) ((list) = arraylist_append(list, data))

#define arraylist_prepend(list, data) ((list) = arraylist_prepend(list, data))

#define arraylist_delete(list, data) ((list) = arraylist_delete(list, data))

#define arraylist_delete_node(slist, node) ((slist) = arraylist_delete_node(slist, node))

#define arraylist_foreach(list, node) for (auto node = (list)->head; node; node = node->next)
