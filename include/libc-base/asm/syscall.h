#pragma once

#ifdef __x86_64__
#  include "syscall-x64"
#else
#  if SYSCALL_USE_INTERRPUT
#    include "syscall-x86"
#  else
#    include "syscall-x86-sysenter"
#  endif
#endif
