#pragma once
#include <define.h>
#include <type.h>
#define BCD_HEX(n) (((n >> 4) * 10) + (n & 0xf))
#define HEX_BCD(n) (((n / 10) << 4) + (n % 10))

#define CMOS_CUR_SEC  0x0
#define CMOS_ALA_SEC  0x1
#define CMOS_CUR_MIN  0x2
#define CMOS_ALA_MIN  0x3
#define CMOS_CUR_HOUR 0x4
#define CMOS_ALA_HOUR 0x5
#define CMOS_WEEK_DAY 0x6
#define CMOS_MON_DAY  0x7
#define CMOS_CUR_MON  0x8
#define CMOS_CUR_YEAR 0x9
#define CMOS_DEV_TYPE 0x12
#define CMOS_CUR_CEN  0x32
#define cmos_index    0x70
#define cmos_data     0x71

u32 get_hour_hex();
u32 get_min_hex();
u32 get_sec_hex();
u32 get_day_of_month();
u32 get_day_of_week();
u32 get_mon_hex();
u32 get_year();