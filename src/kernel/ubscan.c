#include <kernel.h>

typedef struct SourceLocation {
  cstr file;
  u32  line;
  u32  col;
} SourceLocation;

#define ublog(type, file, line, col)                                                               \
  klog(STR_ERROR CRGB(253, 133, 172) "at" CEND " [" CRGB(192, 128, 255) "%24s" CEND "] [" CRGB(    \
           0, 255, 255) "%4u" CEND ":" CRGB(255, 128, 192) "%-3u" CEND "] " COLOR_ERROR            \
                                                           "UB! " type CEND "\n",                  \
       _log_relative_path_((file) ?: "<unknown>"), line, col)

#define HANDLE(_type_)                                                                             \
  __nif void __ubsan_handle_##_type_(const SourceLocation *pos) {                                  \
    asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");                                              \
    ublog(#_type_, pos->file, pos->line, pos->col);                                                \
  }                                                                                                \
  __nif __attr(noreturn) void __ubsan_handle_##_type_##abort(const SourceLocation *pos) {          \
    asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");                                              \
    ublog(#_type_, pos->file, pos->line, pos->col);                                                \
    abort();                                                                                       \
  }

HANDLE(add_overflow);
HANDLE(alignment_assumption);
HANDLE(builtin_unreachable);
HANDLE(cfi_bad_type);
HANDLE(cfi_check_fail);
HANDLE(divrem_overflow);
HANDLE(dynamic_type_cache_miss);
HANDLE(float_cast_overflow);
HANDLE(function_type_mismatch);
HANDLE(implicit_conversion);
HANDLE(invalid_builtin);
HANDLE(invalid_objc_cast);
HANDLE(load_invalid_value);
HANDLE(missing_return);
HANDLE(mul_overflow);
HANDLE(negate_overflow);
HANDLE(nonnull_arg);
HANDLE(nonnull_return_v1);
HANDLE(nullability_arg);
HANDLE(nullability_return_v1);
HANDLE(out_of_bounds);
HANDLE(pointer_overflow);
HANDLE(shift_out_of_bounds);
HANDLE(sub_overflow);
HANDLE(type_mismatch_v1);
HANDLE(vla_bound_not_positive);

__nif void __ubsan_default_options() {
  asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");
  kloge("UB! ");
}

__nif void __ubsan_on_report() {
  asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");
  kloge("UB! ");
}

__nif void __ubsan_get_current_report_data() {
  asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");
  kloge("UB! ");
}
