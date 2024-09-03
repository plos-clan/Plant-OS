#include <benchmark/benchmark.h>
#include <random>

#define NO_STD 0
#include <libc++.hpp>

std::vector<f64> generate_nums(size_t length, ssize_t seed) {
  std::default_random_engine rng(seed);
  std::vector<f64>           result;
  result.reserve(length);
  for (size_t i = 0; i < length; i++) {
    result.push_back(rng() / 65536.);
  }
  return result;
}

std::vector<f64> nums   = generate_nums(100, 1145141919810);
std::vector<f64> nums_x = generate_nums(100, 114514);
std::vector<f64> nums_y = generate_nums(100, 1919810);

static void BM_POW(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums_x) {
      for (const auto y : nums_y) {
        benchmark::DoNotOptimize(cpp::pow(x, y));
      }
    }
  }
}
static void BM_STD_POW(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums_x) {
      for (const auto y : nums_y) {
        benchmark::DoNotOptimize(std::pow(x, y));
      }
    }
  }
}
BENCHMARK(BM_POW);
BENCHMARK(BM_STD_POW);

static void BM_ABS(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(cpp::abs(x));
    }
  }
}
static void BM_STD_ABS(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(std::abs(x));
    }
  }
}
BENCHMARK(BM_ABS);
BENCHMARK(BM_STD_ABS);

static void BM_CLAMP(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(cpp::clamp(x, 0., 1.));
    }
  }
}
static void BM_STD_CLAMP(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(std::clamp(x, 0., 1.));
    }
  }
}
BENCHMARK(BM_CLAMP);
BENCHMARK(BM_STD_CLAMP);

static void BM_SQRT(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(cpp::sqrt(x));
    }
  }
}
static void BM_STD_SQRT(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(std::sqrt(x));
    }
  }
}
BENCHMARK(BM_SQRT);
BENCHMARK(BM_STD_SQRT);

static void BM_CBRT(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(cpp::cbrt(x));
    }
  }
}
static void BM_STD_CBRT(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(std::cbrt(x));
    }
  }
}
BENCHMARK(BM_CBRT);
BENCHMARK(BM_STD_CBRT);

static void BM_FMOD(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums_x) {
      for (const auto y : nums_y) {
        benchmark::DoNotOptimize(cpp::mod(x, y));
      }
    }
  }
}
static void BM_STD_FMOD(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums_x) {
      for (const auto y : nums_y) {
        benchmark::DoNotOptimize(std::fmod(x, y));
      }
    }
  }
}
BENCHMARK(BM_FMOD);
BENCHMARK(BM_STD_FMOD);

static void BM_SIN(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(cpp::sin(x));
    }
  }
}
static void BM_STD_SIN(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(std::sin(x));
    }
  }
}
BENCHMARK(BM_SIN);
BENCHMARK(BM_STD_SIN);

static void BM_TAN(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(cpp::tan(x));
    }
  }
}
static void BM_STD_TAN(benchmark::State &state) {
  for (auto _ : state) {
    for (const auto x : nums) {
      benchmark::DoNotOptimize(std::tan(x));
    }
  }
}
BENCHMARK(BM_TAN);
BENCHMARK(BM_STD_TAN);

BENCHMARK_MAIN();
