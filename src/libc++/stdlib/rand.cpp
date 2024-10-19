#include <libc-base.hpp>

namespace cpp {

class MT19937 {
private:
  u32  mt[624];
  uint idx;

public:
  MT19937(u32 seed) {
    this->idx   = 0;
    this->mt[0] = seed;
    for (uint i = 1; i < 624; i++) {
      u32 n       = this->mt[i - 1] ^ (this->mt[i - 1] >> 30);
      this->mt[i] = 0x6c078965 * n + i;
    }
  }

  void generate() {
    for (uint i = 1; i < 623; i++) {
      u32 y       = (this->mt[i] & 0x80000000) + (this->mt[i + 1] & 0x7fffffff);
      this->mt[i] = (y >> 1) ^ this->mt[(i + 397) % 624];
      if (y & 1) this->mt[i] ^= 0x9908b0df;
    }
    u32 y         = (this->mt[623] & 0x80000000) + (this->mt[0] & 0x7fffffff);
    this->mt[623] = (y >> 1) ^ this->mt[396];
    if (y & 1) this->mt[623] ^= 0x9908b0df;
  }

  u32 extract() {
    if (this->idx >= 624) generate();
    u32 y  = this->mt[this->idx++];
    y     ^= (y >> 11);
    y     ^= (y << 7) & 0x9d2c5680;
    y     ^= (y << 15) & 0xefc60000;
    y     ^= (y >> 18);
    return y;
  }
};

} // namespace cpp
