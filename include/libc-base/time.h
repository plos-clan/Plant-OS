#pragma once
#include <define.h>

#if NO_STD

struct tm {
  int tm_sec;   // 秒 [0-60] (60 是闰秒)
  int tm_min;   // 分 [0-59]
  int tm_hour;  // 时 [0-23]
  int tm_mday;  // 日 [1-31]
  int tm_mon;   // 月 [0-11]
  int tm_year;  // 年 - 1900
  int tm_wday;  // 星期 [0-6] (0 是周日)
  int tm_yday;  // 一年中的第几天 [0-365]
  int tm_isdst; // 夏令时标志 (0 无夏令时, >0 有夏令时, <0 未知)

#  ifdef __USE_MISC
  long int    tm_gmtoff; /* Seconds east of UTC.  */
  const char *tm_zone;   /* Timezone abbreviation.  */
#  else
  long int    __tm_gmtoff; /* Seconds east of UTC.  */
  const char *__tm_zone;   /* Timezone abbreviation.  */
#  endif
};

time_t     mktime(struct tm *tp);
struct tm *localtime(const time_t *time);
struct tm *localtime_r(const time_t *time, struct tm *tp);

#endif
