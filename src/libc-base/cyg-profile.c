#include <libc-base.h>

__attr(weak) __nif void __cyg_profile_func_enter(void *callee, void *caller) {
  // 此函数应为空
}

__attr(weak) __nif void __cyg_profile_func_exit(void *callee, void *caller) {
  // 此函数应为空
}
