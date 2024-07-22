#pragma once
#include <define.h>
#include <type.h>

//* ----------------------------------------------------------------------------------------------------
//; 标准库的
//* ----------------------------------------------------------------------------------------------------

#if NO_STD

// --------------------------------------------------
//; 求和

// --------------------------------------------------
//; 平均数

// --------------------------------------------------
//;

finline int abs(int x) {
  return x >= 0 ? x : -x;
}

finline double fmod(double x, double y) {
  return x - (long)(x / y) * y;
}

finline float fmodf(float x, float y) {
  return x - (int)(x / y) * y;
}

#endif

//* ----------------------------------------------------------------------------------------------------
//; 自定义的
//* ----------------------------------------------------------------------------------------------------

// --------------------------------------------------
//; 最大最小

finline int min(int a, int b) {
  return a < b ? a : b;
}

finline int max(int a, int b) {
  return a > b ? a : b;
}

// --------------------------------------------------
//; 平方 立方

finline int square(int x) {
  return x * x;
}

finline float squaref(float x) {
  return x * x;
}

finline int cube(int x) {
  return x * x * x;
}

finline float cubef(float x) {
  return x * x * x;
}

// --------------------------------------------------
//;

finline int gcd(int a, int b) {
  while (b != 0) {
    int t = b;
    b     = a % b;
    a     = t;
  }
  return a;
}

finline long gcdl(long a, long b) {
  while (b != 0) {
    long t = b;
    b      = a % b;
    a      = t;
  }
  return a;
}

finline u64 factorial(u32 n) {
  u64 result = 1;
  for (u32 i = 2; i <= n; i++) {
    result *= i;
  }
  return result;
}
