#include <loader.h>

#define HANDLE(_type_)                                                                             \
  __nif void __ubsan_handle_##_type_() {                                                           \
    asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");                                              \
  }                                                                                                \
  __nif void __ubsan_handle_##_type_##abort() {                                                    \
    asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");                                              \
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
}

__nif void __ubsan_on_report() {
  asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");
}

__nif void __ubsan_get_current_report_data() {
  asm volatile("xchg %%bx, %%bx\n\t" ::: "memory");
}
