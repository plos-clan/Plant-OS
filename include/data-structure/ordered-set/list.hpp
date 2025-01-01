#pragma once
#include <libc-base.hpp>

namespace cpp {

template <typename T>
struct List {
  struct Node {
    T     data;
    Node *prev = null;
    Node *next = null;

    Node() = default;
    Node(const T &value) : data(value) {}
    Node(T &&value) : data(cpp::move(value)) {}
  };

  class Iterator {
    Node *node = null;

  public:
    Iterator() = default;
    Iterator(Node *node) : node(node) {}

    auto operator++() -> Iterator & {
      if (node) node = node->next;
      return *this;
    }

    auto operator--() -> Iterator & {
      if (node) node = node->prev;
      return *this;
    }

    auto operator==(const Iterator &it) const -> bool {
      return node == it.node;
    }

    auto operator!=(const Iterator &it) const -> bool {
      return node != it.node;
    }
  };

  Node  *head = null;
  Node  *tail = null;
  size_t len  = 0;
  T      errdata;

  List() = default;
  List(Node *head, Node *tail, size_t len) : head(head), tail(tail), len(len) {}
  List(const List &list) {
    *this = list.copy();
  }
  List(List &&list) : head(list.head), tail(list.tail), len(list.len) {
    list.head = null;
    list.tail = null;
    list.len  = 0;
  }
  ~List() {
    clear();
  }

  auto operator=(const List &list) -> List & {
    if (this == &list) return *this;
    *this = list.copy();
    return *this;
  }
  auto operator=(List &&list) -> List & {
    if (this == &list) return *this;
    head      = list.head;
    tail      = list.tail;
    len       = list.len;
    list.head = null;
    list.tail = null;
    list.len  = 0;
    return *this;
  }

  auto operator[](ssize_t i) const -> const T & {
    return at(i);
  }
  auto operator[](ssize_t i) -> T & {
    return at(i);
  }
  auto operator[](size_t i) const -> const T & {
    return at(i);
  }
  auto operator[](size_t i) -> T & {
    return at(i);
  }

  auto copy() -> List && {
    if (len == 0) return {};
    Node *_head = new Node(head->data);
    Node *_tail = _head;
    for (auto *node = head->next; node; node = node->next) {
      auto *_temp = new Node(node->data);
      _tail->next = _temp;
      _temp->prev = _tail;
      _tail       = _temp;
    }
    return {_head, _tail, len};
  }

  auto clear() -> List & {
    while (head != null) {
      auto next = head->next;
      delete head;
      head = next;
    }
    tail = null;
    len  = 0;
    return *this;
  }

  auto clear(void (*callback)(const T &)) -> List & {
    while (head != null) {
      auto next = head->next;
      callback(head->data);
      delete head;
      head = next;
    }
    tail = null;
    len  = 0;
    return *this;
  }

  auto first() const -> const T & {
    return head ? head->data : errdata;
  }

  auto first() -> T & {
    return head ? head->data : errdata;
  }

  auto last() const -> T const & {
    return tail ? tail->data : errdata;
  }

  auto last() -> T & {
    return tail ? tail->data : errdata;
  }

  auto append(Node *node) -> List & {
    len++;

    if (head == null) {
      head = tail = node;
      return *this;
    }

    tail->next = node;
    node->prev = tail;
    tail       = node;

    return *this;
  }

  auto append(const T &data) -> List & {
    return append(new Node(data));
  }

  auto prepend(Node *node) -> List & {
    len++;

    if (head == null) {
      head = tail = node;
      return *this;
    }

    head->prev = node;
    node->next = head;
    head       = node;

    return *this;
  }

  auto prepend(const T &data) -> List & {
    return prepend(new Node(data));
  }

  auto at(ssize_t i) const -> const T & {
    if (i < 0) return errdata;
    return at((size_t)i);
  }

  auto at(ssize_t i) -> T & {
    if (i < 0) return errdata;
    return at((size_t)i);
  }

  auto at(size_t i) const -> const T & {
    if (i >= len) return errdata;
    if (i <= len / 2) {
      ssize_t n = 0;
      for (auto *cur = head; cur; cur = cur->next) {
        if (n == i) return cur->data;
        n++;
      }
    } else {
      ssize_t n = len - 1;
      for (auto *cur = tail; cur; cur = cur->prev) {
        if (n == i) return cur->data;
        n--;
      }
    }
  }

  auto at(size_t i) -> T & {
    if (i >= len) return errdata;
    if (i <= len / 2) {
      ssize_t n = 0;
      for (auto *cur = head; cur; cur = cur->next) {
        if (n == i) return cur->data;
        n++;
      }
    } else {
      ssize_t n = len - 1;
      for (auto *cur = tail; cur; cur = cur->prev) {
        if (n == i) return cur->data;
        n--;
      }
    }
  }

  auto nodeat(size_t i) -> Node * {
    if (i >= len) return null;
    if (i <= len / 2) {
      ssize_t n = 0;
      for (auto *cur = head; cur; cur = cur->next) {
        if (n == i) return cur;
        n++;
      }
    } else {
      ssize_t n = len - 1;
      for (auto *cur = tail; cur; cur = cur->prev) {
        if (n == i) return cur;
        n--;
      }
    }
  }

  auto has(const T &data) -> bool {
    for (auto *cur = head; cur; cur = cur->next) {
      if (cur->data == data) return true;
    }
    return false;
  }

  auto index(const T &data) -> ssize_t {
    ssize_t n = 0;
    for (auto *cur = head; cur; cur = cur->next) {
      if (cur->data == data) return n;
      n++;
    }
    return -1;
  }

  auto nodeof(const T &data) -> Node * {
    for (auto *cur = head; cur; cur = cur->next) {
      if (cur->data == data) return cur;
    }
    return null;
  }

  auto detach(Node *node) -> List & {
    if (node->prev) {
      node->prev->next = node->next;
    } else {
      head = node->next;
    }
    if (node->next) {
      node->next->prev = node->prev;
    } else {
      tail = node->prev;
    }
    node->next = node->prev = null;
    len--;
    return *this;
  }

  auto del(Node *node) -> List & {
    if (head == node) {
      head = head->next;
      if (head == null) tail = null;
      goto do_del;
    }

    if (tail == node) {
      tail = tail->prev;
      if (tail == null) head = null;
      goto do_del;
    }

    node->prev->next = node->next;
    node->next->prev = node->prev;

  do_del:
    delete node;
    len--;
    return *this;
  }

  auto del(const T &data) -> List & {
    for (auto *cur = head; cur; cur = cur->next) {
      if (cur->data == data) {
        del(cur);
        break;
      }
    }
    return *this;
  }

  auto begin() -> Iterator {
    return Iterator(head);
  }

  auto end() -> Iterator {
    return Iterator();
  }

  auto each(void (*callback)(const T &)) -> List & {
    for (auto *cur = head; cur; cur = cur->next) {
      callback(cur->data);
    }
  }

  auto each(void (*callback)(size_t, const T &)) -> List & {
    ssize_t n = 0;
    for (auto *cur = head; cur; cur = cur->next) {
      callback(n, cur->data);
      n++;
    }
  }
};

} // namespace cpp
