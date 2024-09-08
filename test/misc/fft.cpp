#include <benchmark/benchmark.h>
#include <fftw3.h>
#include <random>

#define NO_STD 0
#include <misc.h>

std::vector<cf64> generate_nums(size_t length, ssize_t seed) {
  std::default_random_engine rng(seed);
  std::vector<cf64>          result;
  result.reserve(length);
  for (size_t i = 0; i < length; i++) {
    result.push_back(rng() / 65536. + rng() / 65536. * cpp::I);
  }
  return result;
}

std::vector<cf64> nums   = generate_nums(2048, 114514);
std::vector<cf64> result = std::vector<cf64>(2048, 0);

static void BM_PLOS_FFT(benchmark::State &state) {
  fft(&result[0], &nums[0], 2048, false);
  for (auto _ : state) {
    fft(&result[0], &nums[0], 2048, false);
    benchmark::DoNotOptimize(result[0]);
  }
}
BENCHMARK(BM_PLOS_FFT);

static void BM_FFTW(benchmark::State &state) {
  auto      in   = (fftw_complex *)&nums[0];
  auto      out  = (fftw_complex *)&result[0];
  fftw_plan plan = fftw_plan_dft_1d(2048, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
  fftw_execute(plan);
  for (auto _ : state) {
    fftw_execute(plan);
    benchmark::DoNotOptimize(out[0]);
  }
  fftw_destroy_plan(plan);
}
BENCHMARK(BM_FFTW);

BENCHMARK_MAIN();
