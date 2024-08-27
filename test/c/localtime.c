#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

time_t my_mktime(struct tm *tp) {
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

struct tm *my_localtime_r(const time_t *time, struct tm *tp) {
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

int main() {
  setenv("TZ", "UTC", 1);
  tzset();

  struct tm time;

  while (1) {
    printf("Enter a date (YYYY/MM/DD HH:MM:SS): ");
    scanf("%d/%d/%d %d:%d:%d", &time.tm_year, &time.tm_mon, &time.tm_mday, &time.tm_hour,
          &time.tm_min, &time.tm_sec);
    time.tm_year -= 1900;
    time.tm_mon  -= 1;

    time_t timestamp = mktime(&time);
    printf("Timestamp:   %ld\n", timestamp);
    localtime_r(&timestamp, &time);
    time_t my_timestamp = my_mktime(&time);
    printf("MyTimestamp: %ld\n", my_timestamp);

    if (timestamp != my_timestamp) printf("-- Error\n");

    struct tm result;
    localtime_r(&timestamp, &result);
    printf("Date:   %d-%02d-%02d (%d) %d:%d:%d\n", result.tm_year + 1900, result.tm_mon + 1,
           result.tm_mday, result.tm_wday, result.tm_hour, result.tm_min, result.tm_sec);
    struct tm my_result;
    my_localtime_r(&timestamp, &my_result);
    printf("MyDate: %d-%02d-%02d (%d) %d:%d:%d\n", my_result.tm_year + 1900, my_result.tm_mon + 1,
           my_result.tm_mday, my_result.tm_wday, my_result.tm_hour, my_result.tm_min,
           my_result.tm_sec);

    if (result.tm_year != my_result.tm_year || result.tm_mon != my_result.tm_mon ||
        result.tm_mday != my_result.tm_mday || result.tm_wday != my_result.tm_wday ||
        result.tm_hour != my_result.tm_hour || result.tm_min != my_result.tm_min ||
        result.tm_sec != my_result.tm_sec)
      printf("-- Error\n");

    printf("\n");
  }

  return 0;
}
