#pragma once
#include "stdio/print.h"
#include "stdlib/lib.h"
#include <define.h>

class ErrCode {
private:
  const i32 code;
  bool      readed = false;

public:
  constexpr ErrCode() = delete;
  constexpr ErrCode(i32 code) : code(code) {}
  constexpr ~ErrCode() {
    if (!readed) {
      printf("You should handle the error codes that may be returned by the function.\n");
      abort();
    }
  }
  constexpr ErrCode(const ErrCode &)            = delete;
  constexpr ErrCode(ErrCode &&)                 = delete;
  constexpr ErrCode &operator=(const ErrCode &) = delete;
  constexpr ErrCode &operator=(ErrCode &&)      = delete;

  constexpr operator i32() {
    readed = true;
    return code;
  }
};
