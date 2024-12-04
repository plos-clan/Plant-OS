#include <libc-base.hpp>

namespace cpp {

#include "mpi32.h"

template <size_t nbits>
class mpi {
  static_assert(nbits > 0, "mpi: nbits must be greater than 0");
  static_assert(nbits % 32 == 0, "mpi: nbits must be a multiple of 32");

public:
  static constexpr size_t nbytes = nbits / 8;
  static constexpr size_t len    = nbits / 32;

  u32 data[len];

  mpi() {
    mpi_set0(data, len);
  }

  mpi(const mpi &other) {
    mpi_set(data, other.data, len);
  }

  mpi(const u32 *other) {
    mpi_set(data, other, len);
  }

  mpi(const i32 other) {
    mpi_seti(data, other, len);
  }

  mpi(const u32 other) {
    mpi_setu(data, other, len);
  }

  mpi(const cstr other, int base) {
    mpi_sets(data, len, other, base);
  }

  mpi &operator=(const mpi &other) {
    mpi_set(data, other.data, len);
    return *this;
  }

  mpi &operator=(const u32 *other) {
    mpi_set(data, other, len);
    return *this;
  }

  mpi &operator=(const i32 other) {
    mpi_seti(data, other, len);
    return *this;
  }

  mpi &operator=(const u32 other) {
    mpi_setu(data, other, len);
    return *this;
  }

  mpi &operator=(const cstr other) {
    mpi_sets(data, len, other, 10);
    return *this;
  }

  mpi &operator+=(const mpi &other) {
    mpi_add2(data, other.data, len);
    return *this;
  }

  mpi &operator-=(const mpi &other) {
    mpi_sub2(data, other.data, len);
    return *this;
  }

  mpi &operator*=(const mpi &other) {
    mpi_mul3(data, data, other.data, len);
    return *this;
  }

  mpi &operator/=(const mpi &other) {
    mpi_div3(data, data, other.data, len);
    return *this;
  }

  mpi &operator%=(const mpi &other) {
    mpi_mod3(data, data, other.data, len);
    return *this;
  }

  mpi &operator<<=(const u32 other) {
    mpi_lshift2(data, other, len);
    return *this;
  }

  mpi &operator>>=(const u32 other) {
    mpi_rshift2(data, other, len);
    return *this;
  }

  mpi operator+(const mpi &other) const {
    mpi result  = *this;
    result     += other;
    return result;
  }

  mpi operator-(const mpi &other) const {
    mpi result  = *this;
    result     -= other;
    return result;
  }

  mpi operator*(const mpi &other) const {
    mpi result  = *this;
    result     *= other;
    return result;
  }

  mpi operator/(const mpi &other) const {
    mpi result  = *this;
    result     /= other;
    return result;
  }

  mpi operator%(const mpi &other) const {
    mpi result  = *this;
    result     %= other;
    return result;
  }

  mpi operator<<(const u32 other) const {
    mpi result   = *this;
    result     <<= other;
    return result;
  }

  mpi operator>>(const u32 other) const {
    mpi result   = *this;
    result     >>= other;
    return result;
  }

  bool operator==(const mpi &other) const {
    return mpi_cmp(data, other.data, len) == 0;
  }

  bool operator!=(const mpi &other) const {
    return mpi_cmp(data, other.data, len) != 0;
  }

  bool operator<(const mpi &other) const {
    return mpi_cmp(data, other.data, len) < 0;
  }

  bool operator>(const mpi &other) const {
    return mpi_cmp(data, other.data, len) > 0;
  }

  bool operator<=(const mpi &other) const {
    return mpi_cmp(data, other.data, len) <= 0;
  }

  bool operator>=(const mpi &other) const {
    return mpi_cmp(data, other.data, len) >= 0;
  }
};

} // namespace cpp
