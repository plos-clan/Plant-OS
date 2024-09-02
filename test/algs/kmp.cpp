#include <benchmark/benchmark.h>
#include <cstring>
#include <random>
#include <string>

#define NO_STD 0
#include "algorithm/kmp.h"

std::string generate_string(size_t length, ssize_t seed) {
  const char charset[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  std::default_random_engine      rng(seed);
  std::uniform_int_distribution<> dist(0, sizeof(charset) - 2);
  std::string                     result;
  result.reserve(length);
  for (size_t i = 0; i < length; i++) {
    result += charset[dist(rng)];
  }
  return result;
}

static void BM_KMP(benchmark::State &state) {
  std::string textpp    = generate_string(10000, 114514);
  std::string patternpp = generate_string(100, 1919810);
  cstr        text      = textpp.c_str();
  cstr        pattern   = patternpp.c_str();
  for (auto _ : state) {
    auto result = kmp_search(text, pattern);
    benchmark::DoNotOptimize(result);
  }
}

BENCHMARK(BM_KMP);

BENCHMARK_MAIN();
