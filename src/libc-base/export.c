// 用于导出所有函数
#include <define.h>
#undef finline
#define finline dlexport __attr(used)
#define static  __attr(used)
#include <libc-base.h>