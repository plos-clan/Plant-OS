#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define EVENT_IMPLEMENTATION
#endif

/**
 *\struct Node
 *\brief 队列节点结构
 */
typedef struct event_node *event_node_t;
struct event_node {
  void        *data; /**< 节点数据 */
  event_node_t next; /**< 下一个节点指针 */
};

#ifdef EVENT_IMPLEMENTATION
#  define extern static
#endif

/**
 *\struct Event
 *\brief 队列结构
 */
typedef struct event {
  spin_t       spin;
  event_node_t head; /**< 队列前端指针 */
  event_node_t tail; /**< 队列后端指针 */
  size_t       size; /**< 队列长度 */
} *event_t;

/**
 *\brief 创建一个新的队列
 *\return 新的队列指针
 */
extern event_t event_alloc();

/**
 *\brief 销毁队列并释放内存
 *\param[in] event 队列指针
 */
extern void event_free(event_t event);

/**
 *\brief 将元素入队列
 *\param[in] event 队列指针
 *\param[in] data 入队的元素
 */
extern void event_push(event_t event, void *data);

/**
 *\brief 将元素出队列
 *\param[in] event 队列指针
 *\return 出队的元素
 */
extern void *event_pop(event_t event);

/**
 *\brief 判断队列是否为空
 *\param[in] event 队列指针
 *\return 若队列为空，则返回true，否则返回false
 */
extern bool event_isempty(event_t event);

/**
 *\brief 获取队列的长度
 *\param[in] event 队列指针
 *\return 队列的长度
 */
extern size_t event_size(event_t event);

/**
 *\brief 打印队列中的元素
 *\param[in] event 队列指针
 */
extern void event_print(event_t event);

#ifdef EVENT_IMPLEMENTATION
#  undef extern
#endif

#ifdef EVENT_IMPLEMENTATION

static event_t event_alloc() {
  event_t event = malloc(sizeof(*event));
  if (event == null) return null;
  event->spin = false;
  event->head = null;
  event->tail = null;
  event->size = 0;
  return event;
}

static void event_free(event_t event) {
  if (event == null) return;
  spin_lock(event->spin);

  event_node_t current = event->head;
  while (current != null) {
    event_node_t temp = current;
    current           = current->next;
    free(temp);
  }

  free(event);
}

static void event_push(event_t event, void *data) {
  if (event == null) return;
  spin_lock(event->spin);

  event_node_t node = malloc(sizeof(*node));
  node->data        = data;
  node->next        = null;

  if (event->head == null) {
    event->head = node;
  } else {
    event->tail->next = node;
  }

  event->tail = node;
  event->size++;

  spin_unlock(event->spin);
}

static void *event_pop(event_t event) {
  if (event == null) return null;
  if (event->head == null) return null;
  spin_lock(event->spin);
  if (event->head == null) {
    spin_unlock(event->spin);
    return null;
  }

  event_node_t temp = event->head;
  void        *data = temp->data;
  event->head       = event->head->next;
  free(temp);

  if (event->head == null) event->tail = null;

  event->size--;
  spin_unlock(event->spin);
  return data;
}

static bool event_isempty(event_t event) {
  return event == null || event->head == null;
}

static size_t event_size(event_t event) {
  return (event == null) ? 0 : event->size;
}

static void event_copyto_with(event_t src, event_t dst, void *(*callback)(void *)) {
  for (event_node_t node = src->head; node != null; node = node->next) {
    event_push(dst, callback(node->data));
  }
}

static void event_copyto(event_t src, event_t dst) {
  for (event_node_t node = src->head; node != null; node = node->next) {
    event_push(dst, node->data);
  }
}

static event_t event_copy_with(event_t event, void *(*callback)(void *)) {
  event_t e_new = event_alloc();
  event_copyto_with(event, e_new, callback);
  return e_new;
}

static event_t event_copy(event_t event) {
  event_t e_new = event_alloc();
  event_copyto(event, e_new);
  return e_new;
}

static void event_print(event_t event) {
  spin_lock(event->spin);
  event_node_t current = event->head;
  while (current != null) {
    printf("%p -> ", current->data);
    current = current->next;
  }
  printf("null\n");
  spin_unlock(event->spin);
}

#  undef EVENT_IMPLEMENTATION
#endif
