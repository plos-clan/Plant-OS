#pragma once
#include "../base.h"

#pragma GCC system_header

#ifndef SYMBOLTABlE_CAPACITY_RATIO
#  define SYMBOLTABlE_CAPACITY_RATIO 2
#endif

#ifndef SYMBOLTABLE_MINIMUM_BUCKETS
#  define SYMBOLTABLE_MINIMUM_BUCKETS 32
#endif

#define SYMBOLTABLE_IMPLEMENTATION

#ifdef ALL_IMPLEMENTATION
#  define SYMBOLTABLE_IMPLEMENTATION
#endif

typedef struct st_node *st_node_t;
struct st_node {
  usize     hash;
  cstr      key;
  void     *value;
  st_node_t next;
};

typedef struct st {
  usize      length;   // 当前存储的元素数量
  usize      capacity; // 在下一次扩大前可以存储的最大元素数量
  usize      mask;     // buckets.length -1
  st_node_t *buckets;  // 指向一个数组，数组的每个元素是一个链表，数组的长度是 2^n
} *st_t;

#ifdef SYMBOLTABLE_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的符号表
 *
 *\return 新的符号表
 */
extern st_t st_new0();
/**
 *\brief 创建一个新的符号表
 *
 *\param size     符号表的最小初始大小
 *\return 新的符号表
 */
extern st_t st_new1(usize size);
#define st_new(...) CONCAT(st_new, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

/**
 *\brief 释放符号表
 *
 *\param st     要释放的符号表
 */
extern void st_free(st_t st);

/**
 *\brief 检查符号表中是否存在指定键
 *
 *\param st     符号表
 *\param key    要检查的键
 *\return 如果存在返回 true，否则返回 false
 */
extern bool st_has(st_t st, cstr key);

/**
 *\brief 获取符号表中指定键的值
 *
 *\param st     符号表
 *\param key    要获取值的键
 *\return 键对应的值，如果键不存在返回 NULL
 */
extern void *st_get2(st_t st, cstr key);

/**
 *\brief 获取符号表中指定键的值，如果键不存在返回默认值
 *
 *\param st       符号表
 *\param key      要获取值的键
 *\param _default 键不存在时返回的默认值
 *\return 键对应的值，如果键不存在返回 _default
 */
extern void *st_get3(st_t st, cstr key, void *_default);
#define st_get(...) CONCAT(st_get, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

/**
 *\brief 设置符号表中指定键的值
 *
 *\param st     符号表
 *\param key    要设置值的键
 *\param value  要设置的值
 *\return 旧的值，如果键不存在返回 NULL
 */
extern void *st_set(st_t st, cstr key, void *value);

/**
 *\brief 插入一个新的键值对到符号表
 *
 *\param st     符号表
 *\param key    要插入的键
 *\param value  要插入的值
 *\return 如果插入成功返回 0，如果键已存在返回 -1
 */
extern int st_insert(st_t st, cstr key, void *value);

/**
 *\brief 移除符号表中指定键的值
 *
 *\param st     符号表
 *\param key    要移除值的键
 *\return 被移除的值，如果键不存在返回 NULL
 */
extern void *st_remove2(st_t st, cstr key);

/**
 *\brief 移除符号表中指定键的值，如果键不存在返回默认值
 *
 *\param st       符号表
 *\param key      要移除值的键
 *\param _default 键不存在时返回的默认值
 *\return 被移除的值，如果键不存在返回 _default
 */
extern void *st_remove3(st_t st, cstr key, void *_default);
#define st_remove(...) CONCAT(st_remove, COUNT_ARGS(__VA_ARGS__))(__VA_ARGS__)

#ifdef SYMBOLTABLE_IMPLEMENTATION
#  undef extern
#endif

#ifdef SYMBOLTABLE_IMPLEMENTATION

finline usize st_hash(cstr key) {
  usize hash = 0;
#  ifdef __x86_64__
  for (usize i = 0; key[i] != '\0'; i++) {
    hash ^= key[i] << 16;
    hash ^= hash << 13;
    hash ^= hash >> 7;
    hash ^= hash << 17;
  }
#  else
  for (usize i = 0; key[i] != '\0'; i++) {
    hash ^= key[i] << 8;
    hash ^= hash << 13;
    hash ^= hash >> 17;
    hash ^= hash << 5;
  }
#  endif
  return hash;
}

static st_t st_new0() {
  return st_new1(SYMBOLTABLE_MINIMUM_BUCKETS);
}

static st_t st_new1(usize size) {
  if (size < SYMBOLTABLE_MINIMUM_BUCKETS) size = SYMBOLTABLE_MINIMUM_BUCKETS;
  if (size & (size - 1)) size = 2 << fhsb(size);
  st_t st = malloc(sizeof(struct st));
  if (st == null) return null;
  st->length   = 0;
  st->capacity = size * SYMBOLTABlE_CAPACITY_RATIO;
  st->mask     = size - 1;
  st->buckets  = calloc(size, sizeof(st_node_t));
  if (st->buckets == null) {
    free(st);
    return null;
  }
  return st;
}

static void st_free(st_t st) {
  if (st == null) return;
  for (usize i = 0; i <= st->mask; i++) {
    for (var node = st->buckets[i]; node;) {
      val next = node->next;
      free(node->key);
      free(node);
      node = next;
    }
  }
  free(st->buckets);
  free(st);
}

static bool st_has(st_t st, cstr key) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node = st->buckets[idx]; node; node = node->next) {
    if (node->hash == hash && strcmp(node->key, key) == 0) return true;
  }
  return false;
}

static void *st_get2(st_t st, cstr key) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node = st->buckets[idx]; node; node = node->next) {
    if (node->hash == hash && strcmp(node->key, key) == 0) return node->value;
  }
  return null;
}

static void *st_get3(st_t st, cstr key, void *_default) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node = st->buckets[idx]; node; node = node->next) {
    if (node->hash == hash && strcmp(node->key, key) == 0) return node->value;
  }
  return _default;
}

static void *st_set(st_t st, cstr key, void *value) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node = st->buckets[idx]; node; node = node->next) {
    if (node->hash == hash && strcmp(node->key, key) == 0) {
      var old     = node->value;
      node->value = value;
      return old;
    }
  }
  st_node_t node   = xmalloc(sizeof(struct st_node));
  node->hash       = hash;
  node->key        = strdup(key);
  node->value      = value;
  node->next       = st->buckets[idx];
  st->buckets[idx] = node;
  st->length++;
  return null;
}

static int st_insert(st_t st, cstr key, void *value) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node = st->buckets[idx]; node; node = node->next) {
    if (node->hash == hash && strcmp(node->key, key) == 0) {
      node->value = value;
      return 0;
    }
  }
  st_node_t node = malloc(sizeof(struct st_node));
  if (node == null) return -1;
  node->hash       = hash;
  node->key        = strdup(key);
  node->value      = value;
  node->next       = st->buckets[idx];
  st->buckets[idx] = node;
  st->length++;
  return 0;
}

static void *st_remove2(st_t st, cstr key) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node_p = &st->buckets[idx]; *node_p; node_p = &(*node_p)->next) {
    val node = *node_p;
    if (node->hash == hash && strcmp(node->key, key) == 0) {
      val value = node->value;
      *node_p   = node->next;
      free(node->key);
      free(node);
      st->length--;
      return value;
    }
  }
  return null;
}

static void *st_remove3(st_t st, cstr key, void *_default) {
  val hash = st_hash(key);
  val idx  = hash & st->mask;
  for (var node_p = &st->buckets[idx]; *node_p; node_p = &(*node_p)->next) {
    val node = *node_p;
    if (node->hash == hash && strcmp(node->key, key) == 0) {
      val value = node->value;
      *node_p   = node->next;
      free(node->key);
      free(node);
      st->length--;
      return value;
    }
  }
  return _default;
}

static void st_print(st_t st) {
  for (usize i = 0; i <= st->mask; i++) {
    for (var node = st->buckets[i]; node; node = node->next) {
      printf("%s -> %p\n", node->key, node->value);
    }
  }
}

#  undef SYMBOLTABLE_IMPLEMENTATION
#endif
