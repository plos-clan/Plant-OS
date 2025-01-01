#pragma once
#include <libc-base.hpp>

namespace cpp::base {

/**
 *\class rbtree
 *\brief 红黑树节点
 */
template <typename TKey, typename TVal>
class rbtree {
public:
  enum {
    red,  /**< 红色节点 */
    black /**< 黑色节点 */
  };

  i32     color = black; /**< 节点颜色，取值为 red 或 black */
  TKey    key;           /**< 节点键值 */
  TVal    value;         /**< 节点值 */
  rbtree *left   = null; /**< 左子节点指针 */
  rbtree *right  = null; /**< 右子节点指针 */
  rbtree *parent = null; /**< 父节点指针 */

  explicit rbtree(const TKey &key) : key(key) {}
  rbtree(const TKey &key, const TVal &value) : key(key), value(value) {}
  explicit rbtree(TKey &&key) : key(cpp::move(key)) {}
  rbtree(TKey &&key, const TVal &value) : key(cpp::move(key)), value(value) {}
  rbtree(const TKey &key, TVal &&value) : key(key), value(cpp::move(value)) {}
  rbtree(TKey &&key, TVal &&value) : key(cpp::move(key)), value(cpp::move(value)) {}
  ~rbtree() {
    if (left) delete left;
    if (right) delete right;
  }

  auto get(const TKey &query) -> TVal * {
    if (query < key) return left ? left->get(query) : null;
    if (query > key) return right ? right->get(query) : null;
    return &value;
  }

  auto get(const TKey &query) const -> const TVal * {
    if (query < key) return left ? left->get(query) : null;
    if (query > key) return right ? right->get(query) : null;
    return &value;
  }

  auto getnode(const TKey &query) -> rbtree * {
    if (query < key) return left ? left->getnode(query) : null;
    if (query > key) return right ? right->getnode(query) : null;
    return this;
  }

  auto getnode(const TKey &query) const -> const rbtree * {
    if (query < key) return left ? left->getnode(query) : null;
    if (query > key) return right ? right->getnode(query) : null;
    return this;
  }

  auto getref(const TKey &query, rbtree *&r) -> TVal & {
    rbtree *result;
    r = insert_if_notexist(this, query, result);
    return result->value;
  }

  auto search(const TVal &query) -> TKey * {
    if (query == value) return &key;
    if (left)
      if (val ret = left->search(query); ret) return ret;
    if (right)
      if (val ret = right->search(query); ret) return ret;
    return null;
  }

  auto search(const TVal &query) const -> const TKey * {
    if (query == value) return &key;
    if (left)
      if (val ret = left->search(query); ret) return ret;
    if (right)
      if (val ret = right->search(query); ret) return ret;
    return null;
  }

  auto searchnode(const TVal &query) -> rbtree * {
    if (query == value) return this;
    if (left)
      if (val ret = left->searchnode(query); ret) return ret;
    if (right)
      if (val ret = right->searchnode(query); ret) return ret;
    return null;
  }

  auto searchnode(const TVal &query) const -> const rbtree * {
    if (query == value) return this;
    if (left)
      if (val ret = left->searchnode(query); ret) return ret;
    if (right)
      if (val ret = right->searchnode(query); ret) return ret;
    return null;
  }

  auto min() -> TVal * {
    return left ? left->min() : &value;
  }

  auto min() const -> const TVal * {
    return left ? left->min() : &value;
  }

  auto minnode() -> rbtree * {
    return left ? left->minnode() : this;
  }

  auto minnode() const -> const rbtree * {
    return left ? left->minnode() : this;
  }

  auto max() -> TVal * {
    return right ? right->max() : &value;
  }

  auto max() const -> const TVal * {
    return right ? right->max() : &value;
  }

  auto maxnode() -> rbtree * {
    return right ? right->maxnode() : this;
  }

  auto maxnode() const -> const rbtree * {
    return right ? right->maxnode() : this;
  }

  auto insert(const TKey &key, const TVal &value) -> rbtree * {
    return insert(this, key, value);
  }

  auto ins(const TKey &key, const TVal &value) -> rbtree * {
    return insert(this, key, value);
  }

  auto del(const TKey &key) -> rbtree * {
    return delete_(this, key);
  }

  auto delete_(const TKey &key) -> rbtree * {
    return delete_(this, key);
  }

  void print_inorder(int deepth = 0) {
    if (deepth == 0) printf("In-order traversal of the Red-Black Tree: \n");
    if (left) left->print_inorder(deepth + 1);
    for (int i = 0; i < deepth; i++)
      printf("| ");
    printf("%d %p\n", key, value);
    if (right) right->print_inorder(deepth + 1);
  }

  void print_preorder(int deepth = 0) {
    if (deepth == 0) printf("Pre-order traversal of the Red-Black Tree: \n");
    for (int i = 0; i < deepth; i++)
      printf("| ");
    printf("%d %p\n", key, value);
    if (left) left->print_preorder(deepth + 1);
    if (right) right->print_preorder(deepth + 1);
  }

  void print_postorder(int deepth = 0) {
    if (deepth == 0) printf("Post-order traversal of the Red-Black Tree: \n");
    if (left) left->print_postorder(deepth + 1);
    if (right) right->print_postorder(deepth + 1);
    for (int i = 0; i < deepth; i++)
      printf("| ");
    printf("%d %p\n", key, value);
  }

private:
  static auto left_rotate(rbtree *r, rbtree *x) -> rbtree * {
    auto y   = x->right;
    x->right = y->left;

    if (y->left != null) y->left->parent = x;

    y->parent = x->parent;

    if (x->parent == null)
      r = y;
    else if (x == x->parent->left)
      x->parent->left = y;
    else
      x->parent->right = y;

    y->left   = x;
    x->parent = y;

    return r;
  }

  static auto right_rotate(rbtree *r, rbtree *y) -> rbtree * {
    auto x  = y->left;
    y->left = x->right;

    if (x->right != null) x->right->parent = y;

    x->parent = y->parent;

    if (y->parent == null)
      r = x;
    else if (y == y->parent->left)
      y->parent->left = x;
    else
      y->parent->right = x;

    x->right  = y;
    y->parent = x;

    return r;
  }

  static auto transplant(rbtree *r, rbtree *u, rbtree *v) -> rbtree * {
    if (u->parent == null)
      r = v;
    else if (u == u->parent->left)
      u->parent->left = v;
    else
      u->parent->right = v;

    if (v != null) v->parent = u->parent;

    return r;
  }

  static auto insert_fixup(rbtree *r, rbtree *z) -> rbtree * {
    while (z != r && z->parent->color == red) {
      if (z->parent == z->parent->parent->left) {
        auto y = z->parent->parent->right;
        if (y != null && y->color == red) {
          z->parent->color         = black;
          y->color                 = black;
          z->parent->parent->color = red;
          z                        = z->parent->parent;
        } else {
          if (z == z->parent->right) {
            z = z->parent;
            r = left_rotate(r, z);
          }
          z->parent->color         = black;
          z->parent->parent->color = red;
          r                        = right_rotate(r, z->parent->parent);
        }
      } else {
        auto y = z->parent->parent->left;
        if (y != null && y->color == red) {
          z->parent->color         = black;
          y->color                 = black;
          z->parent->parent->color = red;
          z                        = z->parent->parent;
        } else {
          if (z == z->parent->left) {
            z = z->parent;
            r = right_rotate(r, z);
          }
          z->parent->color         = black;
          z->parent->parent->color = red;
          r                        = left_rotate(r, z->parent->parent);
        }
      }
    }

    r->color = black;
    return r;
  }

  static auto insert(rbtree *r, const TKey &key, const TVal &value) -> rbtree * {
    auto z   = new rbtree(key, value);
    z->color = red;

    rbtree *y = null;
    rbtree *x = r;

    while (x != null) {
      y = x;
      if (key < x->key)
        x = x->left;
      else
        x = x->right;
    }

    z->parent = y;
    if (y == null)
      r = z;
    else if (key < y->key)
      y->left = z;
    else
      y->right = z;

    return insert_fixup(r, z);
  }

  static auto insert_if_notexist(rbtree *r, const TKey &key, rbtree *&z) -> rbtree * {
    rbtree *y = null;
    rbtree *x = r;

    while (x != null) {
      y = x;
      if (key < x->key)
        x = x->left;
      else if (key > x->key)
        x = x->right;
      else {
        z = x;
        return r;
      }
    }

    z         = new rbtree(key);
    z->color  = red;
    z->parent = y;
    if (y == null)
      r = z;
    else if (key < y->key)
      y->left = z;
    else
      y->right = z;

    return insert_fixup(r, z);
  }

  static auto delete_fixup(rbtree *r, rbtree *x, rbtree *x_p) -> rbtree * {
    while (x != r && (x == null || x->color == black)) {
      if (x == x_p->left) {
        auto w = x_p->right;
        if (w->color == red) {
          w->color   = black;
          x_p->color = red;
          r          = left_rotate(r, x_p);
          w          = x_p->right;
        }
        if ((w->left == null || w->left->color == black) &&
            (w->right == null || w->right->color == black)) {
          w->color = red;
          x        = x_p;
          x_p      = x_p->parent;
        } else {
          if (w->right == null || w->right->color == black) {
            if (w->left != null) w->left->color = black;
            w->color = red;
            r        = right_rotate(r, w);
            w        = x_p->right;
          }
          w->color   = x_p->color;
          x_p->color = black;
          if (w->right != null) w->right->color = black;
          r = left_rotate(r, x_p);
          x = r;
        }
      } else {
        auto w = x_p->left;
        if (w->color == red) {
          w->color   = black;
          x_p->color = red;
          r          = right_rotate(r, x_p);
          w          = x_p->left;
        }
        if ((w->right == null || w->right->color == black) &&
            (w->left == null || w->left->color == black)) {
          w->color = red;
          x        = x_p;
          x_p      = x_p->parent;
        } else {
          if (w->left == null || w->left->color == black) {
            if (w->right != null) w->right->color = black;
            w->color = red;
            r        = left_rotate(r, w);
            w        = x_p->left;
          }
          w->color   = x_p->color;
          x_p->color = black;
          if (w->left != null) w->left->color = black;
          r = right_rotate(r, x_p);
          x = r;
        }
      }
    }

    if (x != null) x->color = black;
    return r;
  }

  static auto delete_(rbtree *r, const TKey &key) -> rbtree * {
    if (r == null) return null;
    rbtree *z = r->getnode(key);
    if (z == null) return r;

    rbtree *x;
    rbtree *x_p;
    int32_t o_color = z->color;

    if (z->left == null) {
      x   = z->right;
      x_p = z->parent;
      r   = transplant(r, z, z->right);
    } else if (z->right == null) {
      x   = z->left;
      x_p = z->parent;
      r   = transplant(r, z, z->left);
    } else {
      auto y  = z->right->minnode();
      o_color = y->color;
      x       = y->right;
      x_p     = y;
      if (y->parent == z) {
        if (x != null) x->parent = y;
      } else {
        r        = transplant(r, y, y->right);
        y->right = z->right;
        if (y->right != null) y->right->parent = y;
      }
      r               = transplant(r, z, y);
      y->left         = z->left;
      y->left->parent = y;
      y->color        = z->color;
    }

    z->left = z->right = null;
    delete z;

    if (o_color == black) r = delete_fixup(r, x, x_p);

    return r;
  }
};

} // namespace cpp::base

namespace cpp {

template <typename TKey, typename TVal>
class RBTree {
private:
  base::rbtree<TKey, TVal> *root = null;

public:
  RBTree() = default;
  ~RBTree() {
    delete root;
  }

  auto has(const TKey &query) const -> bool {
    return root ? root->getnode(query) != null : false;
  }

  auto get(const TKey &query) -> TVal & {
    return root ? root->getref(query, root) : (root = new base::rbtree<TKey, TVal>(query))->value;
  }

  auto getnode(const TKey &query) const -> const base::rbtree<TKey, TVal> * {
    return root ? root->getnode(query) : null;
  }

  auto search(const TVal &query) const -> const TKey * {
    return root ? root->search(query) : null;
  }

  auto searchnode(const TVal &query) const -> const base::rbtree<TKey, TVal> * {
    return root ? root->searchnode(query) : null;
  }

  auto min() -> TVal * {
    return root ? root->min() : null;
  }

  auto min() const -> const TVal * {
    return root ? root->min() : null;
  }

  auto minnode() const -> const base::rbtree<TKey, TVal> * {
    return root ? root->minnode() : null;
  }

  auto max() -> TVal * {
    return root ? root->max() : null;
  }

  auto max() const -> const TVal * {
    return root ? root->max() : null;
  }

  auto maxnode() const -> const base::rbtree<TKey, TVal> * {
    return root ? root->maxnode() : null;
  }

  auto insert(const TKey &key, const TVal &value) -> void {
    if (root)
      root = root->insert(key, value);
    else
      root = new base::rbtree<TKey, TVal>(key, value);
  }

  auto ins(const TKey &key, const TVal &value) -> void {
    if (root)
      root = root->insert(key, value);
    else
      root = new base::rbtree<TKey, TVal>(key, value);
  }

  auto del(const TKey &key) -> void {
    if (root) root = root->delete_(key);
  }

  auto delete_(const TKey &key) -> void {
    if (root) root = root->delete_(key);
  }
};

} // namespace cpp
