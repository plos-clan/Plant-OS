// 用于导出所有函数
#include <define.h>
#undef finline
#define finline dlexport
#define static  dlexport
#include <libc-base.h>