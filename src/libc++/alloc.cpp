#include <libc++.hpp>

auto operator new(size_t size) -> void * {
  return malloc(size);
}

auto operator new[](size_t size) -> void * {
  return malloc(size);
}

auto operator new(size_t size, void *ptr) -> void * {
  return ptr;
}

auto operator new[](size_t size, void *ptr) -> void * {
  return ptr;
}

auto operator delete(void *ptr, size_t size) -> void {
  free(ptr);
}

auto operator delete[](void *ptr, size_t size) -> void {
  free(ptr);
}

auto operator delete(void *ptr) -> void {
  free(ptr);
}

auto operator delete[](void *ptr) -> void {
  free(ptr);
}
