#include <kernel.h>

#ifndef PORT
#  warning "PORT is not defined"
#  define PORT 0
#endif

//* ----------------------------------------------------------------------------------------------------
//; 扩展数据输出
//* ----------------------------------------------------------------------------------------------------

finline void log_out8(byte a) {
  waituntil(asm_in8(PORT + 5) & 0x20);
  asm_out8(PORT, a);
}

finline void log_out16(u16 a) {
  log_out8(a);
  log_out8(a >> 8);
}

finline void log_out32(u32 a) {
  log_out16(a);
  log_out16(a >> 16);
}

finline void log_out64(u64 a) {
  log_out32(a);
  log_out32(a >> 32);
}

finline void log_outp(const void *p) {
#if __x86_64__
  log_out64((size_t)p);
#else
  log_out32((size_t)p);
#endif
}

finline void log_outb(const void *ptr, size_t size) {
  for (size_t i = 0; i < size; i++) {
    log_out8(((const byte *)ptr)[i]);
  }
}

finline void log_outs(cstr s) {
  size_t n = 0;
  for (; s[n] != '\0'; n++) {}
  log_out32(n);
  for (size_t i = 0; i < n; i++) {
    log_out8(s[i]);
  }
}

#define log_dump(data) log_outb(&(data), sizeof(data))

//* ----------------------------------------------------------------------------------------------------
//; 日志类型 ID
//* ----------------------------------------------------------------------------------------------------

enum {
  ID_MSG        = 0,
  ID_FUNC_ENTER = 1,
  ID_FUNC_EXIT  = 2,
};

//* ----------------------------------------------------------------------------------------------------
//; 函数调用追踪
//* ----------------------------------------------------------------------------------------------------

// __nif void __cyg_profile_func_enter(void *callee, void *caller) {
//   size_t flags = asm_get_flags();
//   asm_cli;
//   struct __PACKED__ {
//     u32   id;
//     void *callee;
//     void *caller;
//   } data = {ID_FUNC_ENTER, callee, caller};
//   log_dump(data);
//   asm_set_flags(flags);
// }

// __nif void __cyg_profile_func_exit(void *callee, void *caller) {
//   size_t flags = asm_get_flags();
//   asm_cli;
//   struct __PACKED__ {
//     u32   id;
//     void *callee;
//     void *caller;
//   } data = {ID_FUNC_EXIT, callee, caller};
//   log_dump(data);
//   asm_set_flags(flags);
// }

//* ----------------------------------------------------------------------------------------------------
//; 扩展日志记录函数
//* ----------------------------------------------------------------------------------------------------

void klog_raw(cstr s) {
  var flag = asm_get_flags();
  asm_cli;
  log_out32(ID_MSG);
  log_outs("Unknown");
  log_outs("Unknown");
  log_outs("Unknown");
  log_out32(-1);
  log_out32(cpuid_coreid);
  log_out32(-1);
  log_outs(s);
  asm_set_flags(flag);
}

void klog(cstr type, cstr file, cstr func, int line, cstr _rest fmt, ...) {
  var flag = asm_get_flags();
  asm_cli;
  static char buf[4096];
  va_list     va;
  va_start(va, fmt);
  vsprintf(buf, fmt, va);
  va_end(va);
  log_outs(buf);
  asm_set_flags(flag);
}

static char print_buf[4096];

void kprintf(cstr _rest fmt1, cstr _rest fmt2, ...) {
  va_list va1, va2;
  va_start(va1, fmt2);
  va_copy(va2, va1);
  vsprintf(print_buf, fmt1, va1);
  klog_raw(print_buf);
  vsprintf(print_buf, fmt2, va2);
  print(print_buf);
  va_end(va2);
  va_end(va1);
}

int printf(cstr _rest fmt, ...) {
  va_list va;
  va_start(va, fmt);
  int rets = vsprintf(print_buf, fmt, va);
  va_end(va);
  print(print_buf);
#if PLOS_LOGGING_PRINTS
  size_t len = strlen(print_buf);
  while (len > 0 && print_buf[len - 1] == '\n') {
    print_buf[len - 1] = '\0';
    len--;
  }
  klogi("print: %s", print_buf);
#endif
  return rets;
}

void puts(cstr s) {
  print(s);
  print("\n");
#if PLOS_LOGGING_PRINTS
  klogi("print: %s", s);
#endif
}
