#include <benchmark/benchmark.h>
#include <cstring>
#include <random>
#include <string>

#define NO_STD 0
#include <data-structure.hpp>

using cpp::RBTree;

std::vector<i32> keys;

std::vector<i32> generate_nums(size_t length, ssize_t seed) {
  std::default_random_engine rng(seed);
  std::vector<i32>           result;
  result.reserve(length);
  cpp::RBTree<i32, i32> tree;
  for (size_t i = 0; i < length;) {
    i32 key = rng();
    if (tree.getnode(key) == null) {
      result.push_back(key);
      tree.insert(key, 0);
      i++;
    }
  }
  return result;
}

static void BM_INSERT(benchmark::State &state) {
  ::keys = generate_nums(10000, 114514);
  for (auto _ : state) {
    cpp::RBTree<i32, i32> tree;
    for (val key : keys) {
      tree.insert(key, 0);
    }
    benchmark::DoNotOptimize(tree);
  }
}

static void BM_GET(benchmark::State &state) {
  cpp::RBTree<i32, i32> tree;
  for (val key : keys) {
    tree.insert(key, 0);
  }
  for (auto _ : state) {
    for (val key : keys) {
      auto value = tree.get(key);
      benchmark::DoNotOptimize(value);
    }
  }
}

static void BM_DELETE(benchmark::State &state) {
  for (auto _ : state) {
    cpp::RBTree<i32, i32> tree;
    for (val key : keys) {
      tree.insert(key, 0);
    }
    for (val key : keys) {
      tree.del(key);
    }
    benchmark::DoNotOptimize(tree);
  }
}

BENCHMARK(BM_INSERT);
BENCHMARK(BM_GET);
BENCHMARK(BM_DELETE);

BENCHMARK_MAIN();
