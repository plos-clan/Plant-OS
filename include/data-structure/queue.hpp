#pragma once
#include <libc-base.hpp>

namespace cpp {

template <typename T>
struct Queue {

  struct Node {
    T     data;
    Node *next;

    Node() = default;
    Node(const T &data, Node *next = null) : data(data), next(next) {}
    Node(T &&data, Node *next = null) : data(move(data)), next(next) {}
  };

  Node  *head;
  Node  *tail;
  size_t size;

  Queue() : head(null), tail(null), size(0) {}
  ~Queue() {
    while (head != null) {
      Node *temp = head;
      head       = head->next;
      delete temp;
    }
    tail = null;
    size = 0;
  }

  void enqueue(const T &data) {
    Node *node = new Node(data);
    if (tail != null) {
      tail->next = node;
    } else {
      head = node;
    }
    tail = node;
    size++;
  }

  void enqueue(T &&data) {
    Node *node = new Node(data);
    if (tail != null) {
      tail->next = node;
    } else {
      head = node;
    }
    tail = node;
    size++;
  }

  auto dequeue() -> T && {
    if (head == null) return null;
    Node *temp = head;
    T     data = move(head->data);
    head       = head->next;
    if (head == null) tail = null;
    delete temp;
    size--;
    return move(data);
  }

  auto empty() const -> bool {
    return head == null;
  }

  auto length() const -> size_t {
    return size;
  }

  auto operator<<(const T &data) -> Queue & {
    enqueue(data);
    return *this;
  }

  auto operator>>(T &data) -> Queue & {
    data = dequeue();
    return *this;
  }
};

} // namespace cpp
