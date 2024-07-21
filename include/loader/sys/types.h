#pragma once
#include <stddef.h>

#ifdef _USE_32BIT_TIME_T
#  ifdef _WIN64
#    undef _USE_32BIT_TIME_T
#  endif
#endif

#ifndef _TIME32_T_DEFINED
#  define _TIME32_T_DEFINED
typedef long __time32_t;
#endif

#ifndef _TIME_T_DEFINED
#  define _TIME_T_DEFINED
typedef __time32_t time_t;
#endif
#ifndef _INO_T_DEFINED
#  define _INO_T_DEFINED
typedef u16 _ino_t;
#  ifndef NO_OLDNAMES
typedef u16 ino_t;
#  endif
#endif

#ifndef _DEV_T_DEFINED
#  define _DEV_T_DEFINED
typedef u32 _dev_t;
#  ifndef NO_OLDNAMES
typedef u32 dev_t;
#  endif
#endif

#ifndef _PID_T_
#  define _PID_T_
#  ifndef _WIN64
typedef int _pid_t;
#  else
__MINGW_EXTENSION
typedef __int64 _pid_t;
#  endif

#  ifndef NO_OLDNAMES
#    undef pid_t
typedef _pid_t pid_t;
#  endif
#endif /* Not _PID_T_ */

#ifndef _MODE_T_
#  define _MODE_T_
typedef u16 _mode_t;

#  ifndef NO_OLDNAMES
typedef _mode_t mode_t;
#  endif
#endif /* Not _MODE_T_ */

typedef long    off32_t;
typedef off32_t off_t;
typedef u32     useconds_t;

#ifndef _TIMESPEC_DEFINED
#  define _TIMESPEC_DEFINED
struct timespec {
  time_t tv_sec;  /* Seconds */
  long   tv_nsec; /* Nanoseconds */
};

struct itimerspec {
  struct timespec it_interval; /* Timer period */
  struct timespec it_value;    /* Timer expiration */
};
#endif /* _TIMESPEC_DEFINED */

#ifndef _SIGSET_T_
#  define _SIGSET_T_
#  ifdef _WIN64
__MINGW_EXTENSION
typedef u64 _sigset_t;
#  else
typedef u32     _sigset_t;
#  endif

#  ifdef _POSIX
typedef _sigset_t sigset_t;
#  endif
#endif /* Not _SIGSET_T_ */
