#include <libc++.hpp>

auto operator new(size_t size) -> void * {
  return malloc(size);
}

auto operator new[](size_t size) -> void * {
  void *ptr = malloc(size);
  memset(ptr, 0, size);
  return ptr;
}

auto operator new(size_t size, void *ptr) -> void * {
  return ptr;
}

auto operator new[](size_t size, void *ptr) -> void * {
  memset(ptr, 0, size);
  return ptr;
}

auto operator delete(void *ptr, size_t size) -> void {
  //
}

auto operator delete[](void *ptr, size_t size) -> void {
  //
}

auto operator delete(void *ptr) -> void {
  free(ptr);
}

auto operator delete[](void *ptr) -> void {
  free(ptr);
}
