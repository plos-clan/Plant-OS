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

struct tm *localtime_r(const time_t *time, struct tm *tp) {
  time_t t     = *time;
  tp->tm_sec   = t % 60;
  t           /= 60;
  tp->tm_min   = t % 60;
  t           /= 60;
  tp->tm_hour  = t % 24;
  t           /= 24;
  tp->tm_wday  = (t + 4) % 7;
  t           -= 10957;
  tp->tm_year  = t / 146097 * 400;
  t           %= 146097;
  tp->tm_year -= t < 0 ? 400 : 0;
  t           += t < 0 ? 146097 : 0;
  bool b1      = t < 36525;
  tp->tm_year += (t - 1) / 36524 * 100;
  t            = (t - 1) % 36524;
  bool b2      = t < 365;
  tp->tm_year += (t + 1) / 1461 * 4;
  t            = (t + 1) % 1461;
  bool b3      = t < 366;
  tp->tm_year += (t - 1) / 365;
  t            = (t - 1) % 365;
  bool b       = b3 - (b2 && !b1);
  t           += b;
  tp->tm_year += 100;
  tp->tm_yday  = t;
  if (t < 31) {
    tp->tm_mon = 0;
  } else if (t < 59 + b) {
    tp->tm_mon  = 1;
    t          -= 31;
  } else if (t < 90 + b) {
    tp->tm_mon  = 2;
    t          -= 59 + b;
  } else if (t < 120 + b) {
    tp->tm_mon  = 3;
    t          -= 90 + b;
  } else if (t < 151 + b) {
    tp->tm_mon  = 4;
    t          -= 120 + b;
  } else if (t < 181 + b) {
    tp->tm_mon  = 5;
    t          -= 151 + b;
  } else if (t < 212 + b) {
    tp->tm_mon  = 6;
    t          -= 181 + b;
  } else if (t < 243 + b) {
    tp->tm_mon  = 7;
    t          -= 212 + b;
  } else if (t < 273 + b) {
    tp->tm_mon  = 8;
    t          -= 243 + b;
  } else if (t < 304 + b) {
    tp->tm_mon  = 9;
    t          -= 273 + b;
  } else if (t < 334 + b) {
    tp->tm_mon  = 10;
    t          -= 304 + b;
  } else {
    tp->tm_mon  = 11;
    t          -= 334 + b;
  }
  tp->tm_mday = t + 1;
  return tp;
}

#endif
