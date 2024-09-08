#include <benchmark/benchmark.h>
#include <cstring>
#include <random>
#include <string>

#define NO_STD 0
#include "../src/misc/crypto/crc.c"
#include "../src/misc/crypto/md2.c"
#include "../src/misc/crypto/md5.c"
#include "../src/misc/crypto/sha256.c"

std::vector<byte> generate_data(size_t size, ssize_t seed) {
  std::default_random_engine rng(seed);
  std::vector<byte>          result;
  result.reserve(size);
  for (size_t i = 0; i < size; i++) {
    result.push_back(rng());
  }
  return result;
}

std::vector<byte> data;
constexpr size_t  size = 1000000;

static void BM_HASH(benchmark::State &state) {
  data      = generate_data(size, 114514);
  void *ptr = &data[0];
  for (auto _ : state) {
    auto result = memhash(ptr, size);
    benchmark::DoNotOptimize(result);
  }
}

static void BM_CRC16(benchmark::State &state) {
  void *ptr = &data[0];
  for (auto _ : state) {
    auto result = CRC16(ptr, size);
    benchmark::DoNotOptimize(result);
  }
}

static void BM_CRC32(benchmark::State &state) {
  void *ptr = &data[0];
  for (auto _ : state) {
    auto result = CRC32(ptr, size);
    benchmark::DoNotOptimize(result);
  }
}

static void BM_MD2(benchmark::State &state) {
  void *ptr = &data[0];
  for (auto _ : state) {
    MD2_t       hash;
    const void *result = MD2(&hash, ptr, size);
    benchmark::DoNotOptimize(*(byte *)result);
  }
}

static void BM_MD5(benchmark::State &state) {
  void *ptr = &data[0];
  for (auto _ : state) {
    MD5_t       hash;
    const void *result = MD5(&hash, ptr, size);
    benchmark::DoNotOptimize(*(byte *)result);
  }
}

static void BM_SHA256(benchmark::State &state) {
  void *ptr = &data[0];
  for (auto _ : state) {
    SHA256_t    hash;
    const void *result = SHA256(&hash, ptr, size);
    benchmark::DoNotOptimize(*(byte *)result);
  }
}

BENCHMARK(BM_HASH);
BENCHMARK(BM_CRC16);
BENCHMARK(BM_CRC32);
BENCHMARK(BM_MD2);
BENCHMARK(BM_MD5);
BENCHMARK(BM_SHA256);

BENCHMARK_MAIN();
