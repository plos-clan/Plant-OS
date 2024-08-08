#include <libc++.hpp>

auto operator new(size_t size) -> void * {
  return malloc(size);
}

auto operator new[](size_t size) -> void * {
  return malloc(size);
}

auto operator delete(void *ptr) -> void {
  free(ptr);
}

auto operator delete[](void *ptr) -> void {
  free(ptr);
}
