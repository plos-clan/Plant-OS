#pragma once

#ifdef __x86_64__
#  include "syscall-x64"
#else
#  include "syscall-x86"
#endif
