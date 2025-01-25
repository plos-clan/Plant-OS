#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define RBTREE_IMPLEMENTATION
#endif

#ifndef _RBTREE_ENUM_
#  define _RBTREE_ENUM_
enum {
  RBT_RED,  // 红色节点
  RBT_BLACK // 黑色节点
};
#endif

/**
 *\struct rbtree
 *\brief 红黑树节点结构
 */
typedef struct rbtree *rbtree_t;
struct rbtree {
  int32_t  color;  /**< 节点颜色，取值为 RED 或 BLACK */
  int32_t  key;    /**< 节点键值 */
  void    *value;  /**< 节点值 */
  rbtree_t left;   /**< 左子节点指针 */
  rbtree_t right;  /**< 右子节点指针 */
  rbtree_t parent; /**< 父节点指针 */
};

#ifdef RBTREE_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的红黑树节点
 *\param[in] key 节点键值
 *\param[in] value 节点值指针
 *\return 新的红黑树节点指针
 */
extern rbtree_t rbtree_alloc(int32_t key, void *value) __THROW __attr_malloc;

/**
 *\brief 释放红黑树
 *\param[in] root 树的根节点
 */
extern void rbtree_free(rbtree_t root) __THROW;

/**
 *\brief 释放红黑树
 *\param[in] root 树的根节点
 */
extern void rbtree_free_with(rbtree_t root, free_t callback) __THROW;

/**
 *\brief 在红黑树中根据键值查找对应的节点值
 *\param[in] root 树的根节点
 *\param[in] key 要查找的键值
 *\return 若找到对应键值的节点值指针，否则返回NULL
 */
extern void *rbtree_get(rbtree_t root, int32_t key) __THROW;

/**
 *\brief 在红黑树中根据键值查找对应的节点
 *\param[in] root 树的根节点
 *\param[in] key 要查找的键值
 *\return 若找到对应键值的节点指针，否则返回NULL
 */
extern rbtree_t rbtree_get_node(rbtree_t root, int32_t key) __THROW;

/**
 *\brief 在红黑树中查找对应值的键值
 *\param[in] root 树的根节点
 *\param[in] value 要查找的值指针
 *\param[out] key 用于存储键值的指针
 *\return 若找到对应值的键值，则返回true，否则返回false
 */
extern bool rbtree_search(rbtree_t root, void *value, int32_t *key) __THROW;

/**
 *\brief 在红黑树中查找对应值的节点
 *\param[in] root 树的根节点
 *\param[in] value 要查找的值指针
 *\return 若找到对应值的节点指针，否则返回NULL
 */
extern rbtree_t rbtree_search_node(rbtree_t root, void *value) __THROW;

/**
 *\brief 在红黑树中查找最小键值的节点
 *\param[in] root 树的根节点
 *\return 最小键值的节点指针
 */
extern rbtree_t rbtree_min(rbtree_t root) __THROW;

/**
 *\brief 在红黑树中查找最大键值的节点
 *\param[in] root 树的根节点
 *\return 最大键值的节点指针
 */
extern rbtree_t rbtree_max(rbtree_t root) __THROW;

/**
 *\brief 在红黑树中插入节点
 *\param[in] root 树的根节点
 *\param[in] key 节点键值
 *\param[in] value 节点值指针
 *\return 插入节点后的树的根节点
 */
extern rbtree_t rbtree_insert(rbtree_t root, int32_t key, void *value) __THROW __wur;

/**
 *\brief 在红黑树中删除节点
 *\param[in] root 树的根节点
 *\param[in] key 节点键值
 *\return 删除节点后的树的根节点
 */
extern rbtree_t rbtree_delete(rbtree_t root, int32_t key) __THROW __wur;

/**
 *\brief 在红黑树中删除节点
 *\param[in] root 树的根节点
 *\param[in] key 节点键值
 *\return 删除节点后的树的根节点
 */
extern rbtree_t rbtree_delete_with(rbtree_t root, int32_t key, free_t callback) __THROW __wur;

/**
 *\brief 按照中序遍历打印红黑树节点
 *\param[in] root 树的根节点
 *\param[in] depth 当前遍历深度（初始值为0）
 */
extern void rbtree_print_inorder(rbtree_t root, int depth) __THROW;

/**
 *\brief 按照前序遍历打印红黑树节点
 *\param[in] root 树的根节点
 *\param[in] depth 当前遍历深度（初始值为0）
 */
extern void rbtree_print_preorder(rbtree_t root, int depth) __THROW;

/**
 *\brief 按照后序遍历打印红黑树节点
 *\param[in] root 树的根节点
 *\param[in] depth 当前遍历深度（初始值为0）
 */
extern void rbtree_print_postorder(rbtree_t root, int depth) __THROW;

#ifdef RBTREE_IMPLEMENTATION
#  undef extern
#endif

#ifdef RBTREE_IMPLEMENTATION

/**
 *\brief 执行左旋操作
 *\param[in] root 树的根节点
 *\param[in] x 要旋转的节点指针
 *\return 旋转后的树的根节点
 */
static rbtree_t rbtree_left_rotate(rbtree_t root, rbtree_t x) __THROW __wur;

/**
 *\brief 执行右旋操作
 *\param[in] root 树的根节点
 *\param[in] y 要旋转的节点指针
 *\return 旋转后的树的根节点
 */
static rbtree_t rbtree_right_rotate(rbtree_t root, rbtree_t y) __THROW __wur;

/**
 *\brief 替换子树
 *\param[in] root 树的根节点
 *\param[in] u 要替换的子树根节点
 *\param[in] v 替换后的子树根节点
 *\return 替换后的树的根节点
 */
static rbtree_t rbtree_transplant(rbtree_t root, rbtree_t u, rbtree_t v) __THROW __wur;

/**
 *\brief 执行插入操作后修复红黑树性质
 *\param[in] root 树的根节点
 *\param[in] z 新插入的节点指针
 *\return 修复后的树的根节点
 */
static rbtree_t rbtree_insert_fixup(rbtree_t root, rbtree_t z) __THROW __wur;

/**
 *\brief 执行删除操作后修复红黑树性质
 *\param[in] root 树的根节点
 *\param[in] x 被删除节点的替代节点指针
 *\param[in] x_parent 被删除节点的父节点指针
 *\return 修复后的树的根节点
 */
static rbtree_t rbtree_delete_fixup(rbtree_t root, rbtree_t x, rbtree_t x_parent) __THROW __wur;

static rbtree_t rbtree_alloc(int32_t key, void *value) noexcept {
  rbtree_t node = (rbtree_t)malloc(sizeof(*node));
  node->key     = key;
  node->value   = value;
  node->color   = RBT_RED;
  node->left    = null;
  node->right   = null;
  node->parent  = null;
  return node;
}

static void rbtree_free(rbtree_t root) noexcept {
  if (root == null) return;
  rbtree_free(root->left);
  rbtree_free(root->right);
  free(root);
}

static void rbtree_free_with(rbtree_t root, free_t callback) noexcept {
  if (root == null) return;
  rbtree_free_with(root->left, callback);
  rbtree_free_with(root->right, callback);
  callback(root->value);
  free(root);
}

static void *rbtree_get(rbtree_t root, int32_t key) noexcept {
  while (root != null && root->key != key) {
    if (key < root->key)
      root = root->left;
    else
      root = root->right;
  }
  return root ? root->value : null;
}

static rbtree_t rbtree_get_node(rbtree_t root, int32_t key) noexcept {
  while (root != null && root->key != key) {
    if (key < root->key)
      root = root->left;
    else
      root = root->right;
  }
  return root;
}

static bool rbtree_search(rbtree_t root, void *value, int32_t *key) noexcept {
  if (root == null) return false;
  if (root->value == value) {
    *key = root->key;
    return true;
  }
  if (root->left && rbtree_search(root->left, value, key)) return true;
  if (root->right && rbtree_search(root->right, value, key)) return true;
  return false;
}

static rbtree_t rbtree_search_node(rbtree_t root, void *value) noexcept {
  if (root == null) return null;
  if (root->value == value) return root;
  if (root->left) {
    rbtree_t node = rbtree_search_node(root->left, value);
    if (node) return node;
  }
  if (root->right) {
    rbtree_t node = rbtree_search_node(root->right, value);
    if (node) return node;
  }
  return null;
}

static rbtree_t rbtree_min(rbtree_t root) noexcept {
  while (root->left != null)
    root = root->left;
  return root;
}

static rbtree_t rbtree_max(rbtree_t root) noexcept {
  while (root->right != null)
    root = root->right;
  return root;
}

static rbtree_t rbtree_left_rotate(rbtree_t root, rbtree_t x) noexcept {
  rbtree_t y = x->right;
  x->right   = y->left;

  if (y->left != null) y->left->parent = x;

  y->parent = x->parent;

  if (x->parent == null)
    root = y;
  else if (x == x->parent->left)
    x->parent->left = y;
  else
    x->parent->right = y;

  y->left   = x;
  x->parent = y;

  return root;
}

static rbtree_t rbtree_right_rotate(rbtree_t root, rbtree_t y) noexcept {
  rbtree_t x = y->left;
  y->left    = x->right;

  if (x->right != null) x->right->parent = y;

  x->parent = y->parent;

  if (y->parent == null)
    root = x;
  else if (y == y->parent->left)
    y->parent->left = x;
  else
    y->parent->right = x;

  x->right  = y;
  y->parent = x;

  return root;
}

static rbtree_t rbtree_transplant(rbtree_t root, rbtree_t u, rbtree_t v) noexcept {
  if (u->parent == null)
    root = v;
  else if (u == u->parent->left)
    u->parent->left = v;
  else
    u->parent->right = v;

  if (v != null) v->parent = u->parent;

  return root;
}

static rbtree_t rbtree_insert_fixup(rbtree_t root, rbtree_t z) noexcept {
  while (z != root && z->parent->color == RBT_RED) {
    if (z->parent == z->parent->parent->left) {
      rbtree_t y = z->parent->parent->right;
      if (y != null && y->color == RBT_RED) {
        z->parent->color         = RBT_BLACK;
        y->color                 = RBT_BLACK;
        z->parent->parent->color = RBT_RED;
        z                        = z->parent->parent;
      } else {
        if (z == z->parent->right) {
          z    = z->parent;
          root = rbtree_left_rotate(root, z);
        }
        z->parent->color         = RBT_BLACK;
        z->parent->parent->color = RBT_RED;
        root                     = rbtree_right_rotate(root, z->parent->parent);
      }
    } else {
      rbtree_t y = z->parent->parent->left;
      if (y != null && y->color == RBT_RED) {
        z->parent->color         = RBT_BLACK;
        y->color                 = RBT_BLACK;
        z->parent->parent->color = RBT_RED;
        z                        = z->parent->parent;
      } else {
        if (z == z->parent->left) {
          z    = z->parent;
          root = rbtree_right_rotate(root, z);
        }
        z->parent->color         = RBT_BLACK;
        z->parent->parent->color = RBT_RED;
        root                     = rbtree_left_rotate(root, z->parent->parent);
      }
    }
  }

  root->color = RBT_BLACK;
  return root;
}

static rbtree_t rbtree_insert(rbtree_t root, int32_t key, void *value) noexcept {
  rbtree_t z = rbtree_alloc(key, value);

  rbtree_t y = null;
  rbtree_t x = root;

  while (x != null) {
    y = x;
    if (key < x->key)
      x = x->left;
    else
      x = x->right;
  }

  z->parent = y;
  if (y == null)
    root = z;
  else if (key < y->key)
    y->left = z;
  else
    y->right = z;

  return rbtree_insert_fixup(root, z);
}

static rbtree_t rbtree_delete_fixup(rbtree_t root, rbtree_t x, rbtree_t x_parent) noexcept {
  while (x != root && (x == null || x->color == RBT_BLACK)) {
    if (x == x_parent->left) {
      rbtree_t w = x_parent->right;
      if (w->color == RBT_RED) {
        w->color        = RBT_BLACK;
        x_parent->color = RBT_RED;
        root            = rbtree_left_rotate(root, x_parent);
        w               = x_parent->right;
      }
      if ((w->left == null || w->left->color == RBT_BLACK) &&
          (w->right == null || w->right->color == RBT_BLACK)) {
        w->color = RBT_RED;
        x        = x_parent;
        x_parent = x_parent->parent;
      } else {
        if (w->right == null || w->right->color == RBT_BLACK) {
          if (w->left != null) w->left->color = RBT_BLACK;
          w->color = RBT_RED;
          root     = rbtree_right_rotate(root, w);
          w        = x_parent->right;
        }
        w->color        = x_parent->color;
        x_parent->color = RBT_BLACK;
        if (w->right != null) w->right->color = RBT_BLACK;
        root = rbtree_left_rotate(root, x_parent);
        x    = root;
      }
    } else {
      rbtree_t w = x_parent->left;
      if (w->color == RBT_RED) {
        w->color        = RBT_BLACK;
        x_parent->color = RBT_RED;
        root            = rbtree_right_rotate(root, x_parent);
        w               = x_parent->left;
      }
      if ((w->right == null || w->right->color == RBT_BLACK) &&
          (w->left == null || w->left->color == RBT_BLACK)) {
        w->color = RBT_RED;
        x        = x_parent;
        x_parent = x_parent->parent;
      } else {
        if (w->left == null || w->left->color == RBT_BLACK) {
          if (w->right != null) w->right->color = RBT_BLACK;
          w->color = RBT_RED;
          root     = rbtree_left_rotate(root, w);
          w        = x_parent->left;
        }
        w->color        = x_parent->color;
        x_parent->color = RBT_BLACK;
        if (w->left != null) w->left->color = RBT_BLACK;
        root = rbtree_right_rotate(root, x_parent);
        x    = root;
      }
    }
  }

  if (x != null) x->color = RBT_BLACK;
  return root;
}

static rbtree_t rbtree_delete_with(rbtree_t root, int32_t key, free_t callback) noexcept {
  if (root == null) return null;
  rbtree_t z = rbtree_get_node(root, key);
  if (z == null) return root;
  if (callback) callback(z->value);

  rbtree_t x;
  rbtree_t x_parent;
  int32_t  original_color = z->color;

  if (z->left == null) {
    x        = z->right;
    x_parent = z->parent;
    root     = rbtree_transplant(root, z, z->right);
  } else if (z->right == null) {
    x        = z->left;
    x_parent = z->parent;
    root     = rbtree_transplant(root, z, z->left);
  } else {
    rbtree_t y     = rbtree_min(z->right);
    original_color = y->color;
    x              = y->right;
    x_parent       = y;
    if (y->parent == z) {
      if (x != null) x->parent = y;
    } else {
      root     = rbtree_transplant(root, y, y->right);
      y->right = z->right;
      if (y->right != null) y->right->parent = y;
    }
    root            = rbtree_transplant(root, z, y);
    y->left         = z->left;
    y->left->parent = y;
    y->color        = z->color;
  }

  free(z);

  if (original_color == RBT_BLACK) root = rbtree_delete_fixup(root, x, x_parent);

  return root;
}

static rbtree_t rbtree_delete(rbtree_t root, int32_t key) noexcept {
  return rbtree_delete_with(root, key, null);
}

static void rbtree_print_inorder(rbtree_t root, int deepth) noexcept {
  if (deepth == 0) printf("In-order traversal of the Red-Black Tree: \n");
  if (root == null) return;
  rbtree_print_inorder(root->left, deepth + 1);
  for (int i = 0; i < deepth; i++)
    printf("| ");
  printf("%d %p\n", root->key, root->value);
  rbtree_print_inorder(root->right, deepth + 1);
}

static void rbtree_print_preorder(rbtree_t root, int deepth) noexcept {
  if (deepth == 0) printf("Pre-order traversal of the Red-Black Tree: \n");
  if (root == null) return;
  for (int i = 0; i < deepth; i++)
    printf("| ");
  printf("%d %p\n", root->key, root->value);
  rbtree_print_preorder(root->left, deepth + 1);
  rbtree_print_preorder(root->right, deepth + 1);
}

static void rbtree_print_postorder(rbtree_t root, int deepth) noexcept {
  if (deepth == 0) printf("Post-order traversal of the Red-Black Tree: \n");
  if (root == null) return;
  rbtree_print_postorder(root->left, deepth + 1);
  rbtree_print_postorder(root->right, deepth + 1);
  for (int i = 0; i < deepth; i++)
    printf("| ");
  printf("%d %p\n", root->key, root->value);
}

#  undef RBTREE_IMPLEMENTATION
#endif

/**
 *\brief 在红黑树中插入节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 *\param[in] value 节点值指针
 */
#define rbtree_insert(root, key, value) ((root) = rbtree_insert(root, key, value))

/**
 *\brief 在红黑树中删除节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 */
#define rbtree_delete(root, key) ((root) = rbtree_delete(root, key))

/**
 *\brief 在红黑树中删除节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 */
#define rbtree_delete_with(root, key, callback) ((root) = rbtree_delete_with(root, key, callback))
