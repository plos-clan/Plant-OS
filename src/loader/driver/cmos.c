#include <dosldr.h>
u8 read_cmos(u8 p) {
  u8 data;
  asm_out8(cmos_index, p);
  data = asm_in8(cmos_data);
  asm_out8(cmos_index, 0x80);
  return data;
}
void write_cmos(u8 p, u8 data) {
  asm_out8(cmos_index, p);
  asm_out8(cmos_data, data);
  asm_out8(cmos_index, 0x80);
  asm_out8(cmos_data, 0);
}
u32 get_hour_hex() {
  return BCD_HEX(read_cmos(CMOS_CUR_HOUR));
}
u32 get_min_hex() {
  return BCD_HEX(read_cmos(CMOS_CUR_MIN));
}
u32 get_sec_hex() {
  return BCD_HEX(read_cmos(CMOS_CUR_SEC));
}
u32 get_day_of_month() {
  return BCD_HEX(read_cmos(CMOS_MON_DAY));
}
u32 get_day_of_week() {
  return BCD_HEX(read_cmos(CMOS_WEEK_DAY));
}
u32 get_mon_hex() {
  return BCD_HEX(read_cmos(CMOS_CUR_MON));
}
u32 get_year() {
  return (BCD_HEX(read_cmos(CMOS_CUR_CEN)) * 100) + BCD_HEX(read_cmos(CMOS_CUR_YEAR)) - 30 + 2010;
}
void write_cmos_time(u32 year, u8 mon, u8 day, u8 hour, u8 min) {
  write_cmos(CMOS_CUR_HOUR, HEX_BCD(hour));
  write_cmos(CMOS_CUR_MIN, HEX_BCD(min));

  write_cmos(CMOS_CUR_YEAR, HEX_BCD(year % 100));
  // month
  write_cmos(CMOS_CUR_MON, HEX_BCD(mon));
  // day
  write_cmos(CMOS_MON_DAY, HEX_BCD(day));
}