// 用于导出所有函数
#include <define.h>
#undef finline
#define finline dlexport
#define static  dlexport
#include <libc-base.h>
#undef isfinite
#if NO_STD
dlexport double isfinite(double v) {
  return __builtin_isfinite(v);
}
#endif