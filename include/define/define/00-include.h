#include "../config.h"

#if !__clang__ && !__has_builtin(assume)
#  define __builtin_assume(...) ((void)0)
#endif
