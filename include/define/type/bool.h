// This code is released under the MIT License

#pragma once

#pragma GCC system_header

// 神经你 C23 加这鬼玩意干嘛，没活可以咬个打火机
#ifndef __cplusplus
#  if __STDC_VERSION__ < 202311L
#    define bool  _Bool
#    define true  ((bool)1)
#    define false ((bool)0)
#  else
#    define _Bool typeof("谁 TM 让你用 _Bool 的"[0][0])
#  endif
#endif
