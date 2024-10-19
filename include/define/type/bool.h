// This code is released under the MIT License

#pragma once

// 神经你 C23 加这鬼玩意干嘛，没活可以咬个打火机
#if !defined(__cplusplus) && __STDC_VERSION__ < 202300L
#  define bool  _Bool
#  define true  ((bool)1)
#  define false ((bool)0)
#endif
