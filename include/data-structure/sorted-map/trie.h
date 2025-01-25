#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define TRIE_IMPLEMENTATION
#endif

/**
 *\struct trie
 *\brief Trie树节点结构
 */
typedef struct trie *trie_t;
struct trie {
  void   *value;    /**< 如果是单词的结尾，则储存值 */
  trie_t *children; /**< 子节点数组 */
};

#ifdef TRIE_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 释放Trie树节点及其子节点的内存
 *\param[in] root 要释放的Trie树节点
 */
extern void trie_free(trie_t root);

/**
 *\brief 创建一个新的Trie树节点
 *\return 新的Trie树节点指针
 */
extern trie_t trie_alloc() __THROW __wur __attr_dealloc(trie_free, 1);

/**
 *\brief 复制Trie树
 *\param[in] root 要复制的Trie树
 *\return 新的Trie树的根节点指针
 */
extern trie_t trie_dup(trie_t root) __THROW __wur __attr_dealloc(trie_free, 1);

/**
 *\brief 在Trie树中插入一个单词
 *\param[in,out] root Trie树的根节点
 *\param[in] key 要插入的键
 *\param[in] value 要插入的值
 */
extern void trie_insert(trie_t root, const char *key, void *value) __THROW;

/**
 *\brief 在Trie树中查找一个单词
 *\param[in] root Trie树的根节点
 *\param[in] key 要查找的单词
 *\return 若找到对应单词，则返回其值，否则返回NULL
 */
extern void *trie_search(trie_t root, const char *key) __THROW;

/**
 *\brief 在Trie树中查找最长的匹配单词
 *\param[in] root Trie树的根节点
 *\param[in] text 要匹配的文本
 *\return 如果找到匹配单词，则返回其值，否则返回Trie树的根节点的值
 */
extern void *trie_longest_match(trie_t root, const char *text) __THROW;

/**
 *\brief 打印Trie树
 *\param[in] root Trie树的根节点
 */
extern void trie_print(trie_t root) __THROW;

#ifdef TRIE_IMPLEMENTATION
#  undef extern
#endif

#ifdef TRIE_IMPLEMENTATION

static trie_t trie_alloc() {
  trie_t node    = malloc(sizeof(*node));
  node->value    = null;
  node->children = null;
  return node;
}

static void trie_free(trie_t root) {
  if (root == null) return;

  if (root->children != null) {
    for (int i = 0; i < 16; i++) {
      if (root->children[i] != null) {
        trie_free(&root->children[i][0]);
        free(root->children[i]);
      }
    }
    free(root->children);
  }

  free(root);
}

static trie_t _trie_dup(trie_t new_root, trie_t root) {
  if (root == null) return null;

  if (new_root == null) new_root = trie_alloc();

  new_root->value = root->value;

  if (root->children != null) {
    new_root->children = malloc(sizeof(trie_t) * 16);
    memset(new_root->children, 0, sizeof(trie_t) * 16);

    for (int i = 0; i < 16; i++) {
      if (root->children[i] == null) continue;
      new_root->children[i] = malloc(sizeof(struct trie) * 16);
      for (int j = 0; j < 16; j++) {
        _trie_dup(&new_root->children[i][j], &root->children[i][j]);
      }
    }
  }

  return new_root;
}

static trie_t trie_dup(trie_t root) {
  return _trie_dup(null, root);
}

static void trie_insert(trie_t root, const char *key, void *value) {
  if (root == null || key == null) return;
  trie_t node = root;

  for (; *key; key++) {
    uint8_t c  = *key;
    uint8_t ch = (c & 0xf0) >> 4, cl = c & 0x0f;
    if (node->children == null) {
      node->children = malloc(sizeof(trie_t) * 16);
      memset(node->children, 0, sizeof(trie_t) * 16);
    }
    if (node->children[ch] == null) {
      node->children[ch] = malloc(sizeof(struct trie) * 16);
      memset(node->children[ch], 0, sizeof(struct trie) * 16);
    }
    node = &node->children[ch][cl];
  }

  node->value = value;
}

static void *trie_search(trie_t root, const char *key) {
  if (root == null || key == null) return null;
  trie_t node = root;

  for (; *key; key++) {
    uint8_t c  = *key;
    uint8_t ch = (c & 0xf0) >> 4, cl = c & 0x0f;
    if (node->children == null) return null;
    if (node->children[ch] == null) return null;
    node = &node->children[ch][cl];
  }

  return node->value;
}

static void *trie_longest_match(trie_t root, const char *text) {
  if (root == null || text == null) return null;
  trie_t node  = root;
  void  *match = root->value;

  for (; *text; text++) {
    uint8_t c  = *text;
    uint8_t ch = (c & 0xf0) >> 4, cl = c & 0x0f;
    if (node->children == null) return match;
    if (node->children[ch] == null) return match;
    node = &node->children[ch][cl];
    if (node->value) match = node->value;
  }

  return match;
}

static void _trie_print(char *buf, int p, trie_t root) {
  if (root->value != null) printf("%s\t%p\n", buf, root->value);

  if (root->children == null) return;

  for (int i = 0; i < 16; i++) {
    if (root->children[i] == null) continue;
    for (int j = 0; j < 16; j++) {
      uint8_t c = (i << 4) + j;
      buf[p]    = c;
      _trie_print(buf, p + 1, &root->children[i][j]);
    }
  }

  buf[p] = '\0';
}

static void trie_print(trie_t root) {
  if (root == null) return;
  char buf[256] = {};
  _trie_print(buf, 0, root);
}

#endif
