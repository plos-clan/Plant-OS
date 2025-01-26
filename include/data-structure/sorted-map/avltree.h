#pragma once
#include "../base.h"

#pragma GCC system_header

#ifdef ALL_IMPLEMENTATION
#  define AVL_IMPLEMENTATION
#endif

typedef struct avltree *avltree_t;
struct avltree {
  int32_t   height; // 节点高度
  int32_t   key;    // 节点键值
  void     *value;  // 节点值
  avltree_t left;   // 左子节点指针
  avltree_t right;  // 右子节点指针
};

#ifdef AVL_IMPLEMENTATION
#  define extern static
#endif

/**
 *\brief 创建一个新的AVL树节点
 *\param[in] key 节点键值
 *\param[in] value 节点值指针
 *\return 新的AVL树节点指针
 */
extern avltree_t avltree_alloc(int32_t key, void *value) __THROW __attr_malloc;

/**
 *\brief 释放AVL树
 *\param[in] root 树的根节点
 */
extern void avltree_free(avltree_t root) __THROW;

/**
 *\brief 释放AVL树，并调用回调函数处理节点值
 *\param[in] root 树的根节点
 *\param[in] callback 回调函数指针，用于处理节点值
 */
extern void avltree_free_with(avltree_t root, void (*callback)(void *)) __THROW;

/**
 *\brief 获取AVL树中指定键值的节点值指针
 *\param[in] root 树的根节点
 *\param[in] key 节点键值
 *\return 节点值指针（若键值不存在，则返回NULL）
 */
extern void *avltree_get(avltree_t root, int32_t key) __THROW;

/**
 *\brief 获取AVL树中指定键值的节点指针
 *\param[in] root 树的根节点
 *\param[in] key 节点键值
 *\return 节点指针（若键值不存在，则返回NULL）
 */
extern avltree_t avltree_get_node(avltree_t root, int32_t key) __THROW;

/**
 *\brief 在AVL树中搜索指定值，并返回对应的键值
 *\param[in] root 树的根节点
 *\param[in] value 指定的节点值指针
 *\param[out] key 存储节点键值的指针（若节点值不存在，则保持不变）
 *\return 若节点值存在于树中，则返回true；否则返回false
 */
extern bool avltree_search(avltree_t root, void *value, int32_t *key) __THROW;

/**
 *\brief 在AVL树中搜索指定值，并返回对应的节点指针
 *\param[in] root 树的根节点
 *\param[in] value 指定的节点值指针
 *\return 节点指针（若节点值不存在，则返回NULL）
 */
extern avltree_t avltree_search_node(avltree_t root, void *value) __THROW;

/**
 *\brief 获取AVL树中的最小节点
 *\param[in] root 树的根节点
 *\return 最小节点指针（若树为空，则返回NULL）
 */
extern avltree_t avltree_min(avltree_t root) __THROW;

/**
 *\brief 获取AVL树中的最大节点
 *\param[in] root 树的根节点
 *\return 最大节点指针（若树为空，则返回NULL）
 */
extern avltree_t avltree_max(avltree_t root) __THROW;

/**
 *\brief 在AVL树中插入节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 *\param[in] value 节点值指针
 *\return 更新后的树的根节点指针
 */
extern avltree_t avltree_insert(avltree_t root, int32_t key, void *value) __THROW __wur;

/**
 *\brief 在AVL树中删除指定键值的节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 *\return 更新后的树的根节点指针
 */
extern avltree_t avltree_delete(avltree_t root, int32_t key) __THROW __wur;

/**
 *\brief 在AVL树中删除指定键值的节点，并调用回调函数处理节点值
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 *\param[in] callback 回调函数指针，用于处理节点值
 *\return 更新后的树的根节点指针
 */
extern avltree_t avltree_delete_with(avltree_t root, int32_t key, void (*callback)(void *)) __THROW
__wur;

/**
 *\brief 按照中序遍历顺序打印AVL树节点
 *\param[in] root 树的根节点
 *\param[in] depth 递归深度（用于缩进打印）
 */
extern void avltree_print_inorder(avltree_t root, int depth) __THROW;

/**
 *\brief 按照前序遍历顺序打印AVL树节点
 *\param[in] root 树的根节点
 *\param[in] depth 递归深度（用于缩进打印）
 */
extern void avltree_print_preorder(avltree_t root, int depth) __THROW;

/**
 *\brief 按照后序遍历顺序打印AVL树节点
 *\param[in] root 树的根节点
 *\param[in] depth 递归深度（用于缩进打印）
 */
extern void avltree_print_postorder(avltree_t root, int depth) __THROW;

#ifdef AVL_IMPLEMENTATION
#  undef extern
#endif

#ifdef AVL_IMPLEMENTATION

static int avltree_height(avltree_t node) noexcept {
  return node == null ? -1 : node->height;
}

static int avltree_max_height(int height1, int height2) noexcept {
  return height1 > height2 ? height1 : height2;
}

static int avltree_balance_factor(avltree_t node) noexcept {
  return node == null ? 0 : avltree_height(node->left) - avltree_height(node->right);
}

static void avltree_update_height(avltree_t node) noexcept {
  if (node != null) {
    int left_height  = avltree_height(node->left);
    int right_height = avltree_height(node->right);
    node->height     = 1 + avltree_max_height(left_height, right_height);
  }
}

static avltree_t avltree_left_rotate(avltree_t node) noexcept {
  avltree_t new_root = node->right;
  node->right        = new_root->left;
  new_root->left     = node;

  avltree_update_height(node);
  avltree_update_height(new_root);

  return new_root;
}

static avltree_t avltree_right_rotate(avltree_t node) noexcept {
  avltree_t new_root = node->left;
  node->left         = new_root->right;
  new_root->right    = node;

  avltree_update_height(node);
  avltree_update_height(new_root);

  return new_root;
}

static avltree_t avltree_balance(avltree_t node) noexcept {
  if (node == null) return null;

  avltree_update_height(node);

  int balance = avltree_balance_factor(node);

  if (balance > 1) {
    if (avltree_balance_factor(node->left) < 0) { node->left = avltree_left_rotate(node->left); }
    return avltree_right_rotate(node);
  }

  if (balance < -1) {
    if (avltree_balance_factor(node->right) > 0) {
      node->right = avltree_right_rotate(node->right);
    }
    return avltree_left_rotate(node);
  }

  return node;
}

static avltree_t avltree_alloc(int32_t key, void *value) noexcept {
  avltree_t node = (avltree_t)malloc(sizeof(*node));
  node->key      = key;
  node->value    = value;
  node->left     = null;
  node->right    = null;
  node->height   = 0;
  return node;
}

static void avltree_free(avltree_t root) noexcept {
  if (root == null) return;
  avltree_free(root->left);
  avltree_free(root->right);
  free(root);
}

static void avltree_free_with(avltree_t root, void (*callback)(void *)) noexcept {
  if (root == null) return;
  avltree_free_with(root->left, callback);
  avltree_free_with(root->right, callback);
  if (callback) callback(root->value);
  free(root);
}

static void *avltree_get(avltree_t root, int32_t key) noexcept {
  while (root != null && root->key != key) {
    if (key < root->key)
      root = root->left;
    else
      root = root->right;
  }
  return root ? root->value : null;
}

static avltree_t avltree_get_node(avltree_t root, int32_t key) noexcept {
  while (root != null && root->key != key) {
    if (key < root->key)
      root = root->left;
    else
      root = root->right;
  }
  return root;
}

static bool avltree_search(avltree_t root, void *value, int32_t *key) noexcept {
  if (root == null) return false;
  if (root->value == value) {
    *key = root->key;
    return true;
  }
  if (root->left && avltree_search(root->left, value, key)) return true;
  if (root->right && avltree_search(root->right, value, key)) return true;
  return false;
}

static avltree_t avltree_search_node(avltree_t root, void *value) noexcept {
  if (root == null) return null;
  if (root->value == value) return root;
  if (root->left) {
    avltree_t node = avltree_search_node(root->left, value);
    if (node) return node;
  }
  if (root->right) {
    avltree_t node = avltree_search_node(root->right, value);
    if (node) return node;
  }
  return null;
}

static avltree_t avltree_min(avltree_t root) noexcept {
  while (root->left != null)
    root = root->left;
  return root;
}

static avltree_t avltree_max(avltree_t root) noexcept {
  while (root->right != null)
    root = root->right;
  return root;
}

static avltree_t avltree_insert(avltree_t root, int32_t key, void *value) noexcept {
  if (root == null) return avltree_alloc(key, value);

  if (key < root->key) {
    root->left = avltree_insert(root->left, key, value);
  } else if (key > root->key) {
    root->right = avltree_insert(root->right, key, value);
  } else {
    root->value = value;
    return root;
  }

  return avltree_balance(root);
}

static avltree_t avltree_delete(avltree_t root, int32_t key) noexcept {
  if (root == null) { return null; }

  if (key < root->key) {
    root->left = avltree_delete(root->left, key);
  } else if (key > root->key) {
    root->right = avltree_delete(root->right, key);
  } else {
    if (root->left == null) {
      avltree_t temp = root->right;
      free(root);
      return temp;
    } else if (root->right == null) {
      avltree_t temp = root->left;
      free(root);
      return temp;
    } else {
      avltree_t temp = avltree_min(root->right);
      root->key      = temp->key;
      root->value    = temp->value;
      root->right    = avltree_delete(root->right, temp->key);
    }
  }

  return avltree_balance(root);
}

static avltree_t avltree_delete_with(avltree_t root, int32_t key,
                                     void (*callback)(void *)) noexcept {
  if (root == null) { return null; }

  if (key < root->key) {
    root->left = avltree_delete_with(root->left, key, callback);
  } else if (key > root->key) {
    root->right = avltree_delete_with(root->right, key, callback);
  } else {
    if (root->left == null) {
      if (callback) callback(root->value);
      avltree_t temp = root->right;
      free(root);
      return temp;
    } else if (root->right == null) {
      if (callback) callback(root->value);
      avltree_t temp = root->left;
      free(root);
      return temp;
    } else {
      if (callback) callback(root->value);
      avltree_t temp = avltree_min(root->right);
      root->key      = temp->key;
      root->value    = temp->value;
      root->right    = avltree_delete(root->right, temp->key);
    }
  }

  return avltree_balance(root);
}

static void avltree_print_inorder(avltree_t root, int depth) noexcept {
  if (depth == 0) printf("In-order traversal of the AVL Tree: \n");
  if (root == null) return;
  avltree_print_inorder(root->left, depth + 1);
  for (int i = 0; i < depth; i++)
    printf("| ");
  printf("%d %p\n", root->key, root->value);
  avltree_print_inorder(root->right, depth + 1);
}

static void avltree_print_preorder(avltree_t root, int depth) noexcept {
  if (depth == 0) printf("Pre-order traversal of the AVL Tree: \n");
  if (root == null) return;
  for (int i = 0; i < depth; i++)
    printf("| ");
  printf("%d %p\n", root->key, root->value);
  avltree_print_preorder(root->left, depth + 1);
  avltree_print_preorder(root->right, depth + 1);
}

static void avltree_print_postorder(avltree_t root, int depth) noexcept {
  if (depth == 0) printf("Post-order traversal of the AVL Tree: \n");
  if (root == null) return;
  avltree_print_postorder(root->left, depth + 1);
  avltree_print_postorder(root->right, depth + 1);
  for (int i = 0; i < depth; i++)
    printf("| ");
  printf("%d %p\n", root->key, root->value);
}

#  undef AVL_IMPLEMENTATION
#endif

/**
 *\brief 在AVL树中插入节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 *\param[in] value 节点值指针
 */
#define avltree_insert(root, key, value) ((root) = avltree_insert(root, key, value))

/**
 *\brief 在AVL树中删除节点
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 */
#define avltree_delete(root, key) ((root) = avltree_delete(root, key))

/**
 *\brief 在AVL树中删除指定键值的节点，并调用回调函数处理节点值
 *\param[in,out] root 树的根节点
 *\param[in] key 节点键值
 *\param[in] callback 回调函数指针，用于处理节点值
 */
#define avltree_delete_with(root, key, callback) ((root) = avltree_delete_with(root, key, callback))
