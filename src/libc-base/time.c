#include <libc-base.h>

#if NO_STD

time_t mktime(struct tm *tp) {
  time_t time  = 0;
  time        += tp->tm_sec;
  time        += tp->tm_min * 60;
  time        += tp->tm_hour * 3600;
  time        += tp->tm_yday * 86400;
  time        += (tp->tm_year - 70) * (time_t)31536000;
  time        += ((tp->tm_year - 69) / 4) * (time_t)86400;
  time        -= ((tp->tm_year - 1) / 100) * (time_t)86400;
  time        += ((tp->tm_year + 299) / 400) * (time_t)86400;
  return time;
}

#endif
