#include <benchmark/benchmark.h>
#include <cstring>
#include <random>

#define NO_STD 0
#define AVL_IMPLEMENTATION
extern "C" {
#include <data-structure/sorted-map/avltree.h>
}

std::vector<i32> keys;

std::vector<i32> generate_nums(size_t length, ssize_t seed) {
  std::default_random_engine rng(seed);
  std::vector<i32>           result;
  result.reserve(length);
  avltree_t tree = null;
  for (size_t i = 0; i < length;) {
    i32 key = rng();
    if (avltree_get_node(tree, key) == null) {
      result.push_back(key);
      avltree_insert(tree, key, null);
      i++;
    }
  }
  avltree_free(tree);
  return result;
}

static void BM_INSERT(benchmark::State &state) {
  ::keys = generate_nums(10000, 114514);
  for (auto _ : state) {
    avltree_t tree = null;
    for (const auto key : keys) {
      avltree_insert(tree, key, null);
    }
    benchmark::DoNotOptimize(tree);
    avltree_free(tree);
  }
}

static void BM_GET(benchmark::State &state) {
  avltree_t tree = null;
  for (const auto key : keys) {
    avltree_insert(tree, key, null);
  }
  for (auto _ : state) {
    for (const auto key : keys) {
      auto val = avltree_get(tree, key);
      benchmark::DoNotOptimize(val);
    }
  }
  avltree_free(tree);
}

static void BM_DELETE(benchmark::State &state) {
  for (auto _ : state) {
    avltree_t tree = null;
    for (const auto key : keys) {
      avltree_insert(tree, key, null);
    }
    for (const auto key : keys) {
      avltree_delete(tree, key);
    }
    benchmark::DoNotOptimize(tree);
  }
}

BENCHMARK(BM_INSERT);
BENCHMARK(BM_GET);
BENCHMARK(BM_DELETE);

BENCHMARK_MAIN();
