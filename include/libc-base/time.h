#pragma once
#include <define.h>

#define MICROSEC_IN_SEC 1000000
#define NANOSEC_IN_SEC  1000000000

typedef struct timeval  timeval;
typedef struct timespec timespec;

#if NO_STD

struct timeval {
  time_t tv_sec;  // 秒
  time_t tv_usec; // 微秒
};

struct timespec {
  time_t tv_sec;  // 秒
  time_t tv_nsec; // 纳秒
};

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

  // GCC 搞啥啊
#  ifdef __USE_MISC
  long int    tm_gmtoff; /* Seconds east of UTC.  */
  const char *tm_zone;   /* Timezone abbreviation.  */
#  else
  long int    __tm_gmtoff; /* Seconds east of UTC.  */
  const char *__tm_zone;   /* Timezone abbreviation.  */
#  endif
};

/**
 *\brief 将时间结构转换为时间戳
 *
 *\param tp 指向时间结构的指针
 *\return time_t 返回时间戳
 */
time_t mktime(struct tm *tp);

/**
 *\brief 将时间戳转换为本地时间结构
 *
 *\param time 指向时间戳的指针
 *\return struct tm* 返回指向本地时间结构的指针
 */
struct tm *localtime(const time_t *time);

/**
 *\brief 将时间戳转换为本地时间结构（线程安全）
 *
 *\param time 指向时间戳的指针
 *\param tp 指向存储结果的时间结构的指针
 *\return struct tm* 返回指向本地时间结构的指针
 */
struct tm *localtime_r(const time_t *time, struct tm *tp);

#endif
