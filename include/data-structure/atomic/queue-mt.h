#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define QUEUE_MT_IMPLEMENTATION
#endif

/**
 *\struct Node
 *\brief 队列节点结构
 */
typedef struct queue_mt_node *queue_mt_node_t;
struct queue_mt_node {
  void           *data; /**< 节点数据 */
  queue_mt_node_t next; /**< 下一个节点指针 */
};

#ifdef QUEUE_MT_IMPLEMENTATION
#  define extern static
#endif

/**
 *\struct Queue
 *\brief 队列结构
 */
typedef struct queue_mt {
  spin_t          spin;
  queue_mt_node_t head; /**< 队列前端指针 */
  queue_mt_node_t tail; /**< 队列后端指针 */
  size_t          size; /**< 队列长度 */
} *queue_mt_t;

/**
 *\brief 创建一个新的队列
 *\return 新的队列指针
 */
extern queue_mt_t queue_mt_alloc();

/**
 *\brief 销毁队列并释放内存
 *\param[in] queue_mt 队列指针
 */
extern void queue_mt_free(queue_mt_t queue_mt);

/**
 *\brief 将元素入队列
 *\param[in] queue_mt 队列指针
 *\param[in] data 入队的元素
 */
extern void queue_mt_enqueue(queue_mt_t queue_mt, void *data);

/**
 *\brief 将元素出队列
 *\param[in] queue_mt 队列指针
 *\return 出队的元素
 */
extern void *queue_mt_dequeue(queue_mt_t queue_mt);

/**
 *\brief 判断队列是否为空
 *\param[in] queue_mt 队列指针
 *\return 若队列为空，则返回true，否则返回false
 */
extern bool queue_mt_isempty(queue_mt_t queue_mt);

/**
 *\brief 获取队列的长度
 *\param[in] queue_mt 队列指针
 *\return 队列的长度
 */
extern size_t queue_mt_size(queue_mt_t queue_mt);

/**
 *\brief 打印队列中的元素
 *\param[in] queue_mt 队列指针
 */
extern void queue_mt_print(queue_mt_t queue_mt);

#ifdef QUEUE_MT_IMPLEMENTATION
#  undef extern
#endif

#ifdef QUEUE_MT_IMPLEMENTATION

static queue_mt_t queue_mt_alloc() {
  queue_mt_t queue_mt = malloc(sizeof(*queue_mt));
  if (queue_mt == null) return null;
  queue_mt->spin = false;
  queue_mt->head = null;
  queue_mt->tail = null;
  queue_mt->size = 0;
  return queue_mt;
}

static void queue_mt_free(queue_mt_t queue_mt) {
  if (queue_mt == null) return;
  spin_lock(queue_mt->spin);

  queue_mt_node_t current = queue_mt->head;
  while (current != null) {
    queue_mt_node_t temp = current;
    current              = current->next;
    free(temp);
  }

  free(queue_mt);
}

static void queue_mt_enqueue(queue_mt_t queue, void *data) {
  if (queue == null) return;
  spin_lock(queue->spin);

  queue_mt_node_t node = malloc(sizeof(*node));
  node->data           = data;
  node->next           = null;

  if (queue->head == null) {
    queue->head = node;
  } else {
    queue->tail->next = node;
  }

  queue->tail = node;
  queue->size++;

  spin_unlock(queue->spin);
}

static void *queue_mt_dequeue(queue_mt_t queue) {
  if (queue == null) return null;
  if (queue->head == null) return null;
  spin_lock(queue->spin);

  queue_mt_node_t temp = queue->head;
  void           *data = temp->data;
  queue->head          = queue->head->next;
  free(temp);

  if (queue->head == null) queue->tail = null;

  queue->size--;
  spin_unlock(queue->spin);
  return data;
}

static bool queue_mt_isempty(queue_mt_t queue) {
  return queue == null || queue->head == null;
}

static size_t queue_mt_size(queue_mt_t queue) {
  return (queue == null) ? 0 : queue->size;
}

static void queue_mt_print(queue_mt_t queue) {
  spin_lock(queue->spin);
  queue_mt_node_t current = queue->head;
  while (current != null) {
    printf("%p -> ", current->data);
    current = current->next;
  }
  printf("null\n");
  spin_unlock(queue->spin);
}

#  undef QUEUE_MT_IMPLEMENTATION
#endif
