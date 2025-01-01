#include <benchmark/benchmark.h>
#include <cstring>
#include <random>

#define NO_STD 0
#define RBTREE_IMPLEMENTATION
extern "C" {
#include <data-structure/sorted-map/rbtree.h>
}

std::vector<i32> keys;

std::vector<i32> generate_nums(size_t length, ssize_t seed) {
  std::default_random_engine rng(seed);
  std::vector<i32>           result;
  result.reserve(length);
  rbtree_t tree = null;
  for (size_t i = 0; i < length;) {
    i32 key = rng();
    if (rbtree_get_node(tree, key) == null) {
      result.push_back(key);
      rbtree_insert(tree, key, null);
      i++;
    }
  }
  rbtree_free(tree);
  return result;
}

static void BM_INSERT(benchmark::State &state) {
  ::keys = generate_nums(10000, 114514);
  for (auto _ : state) {
    rbtree_t tree = null;
    for (val key : keys) {
      rbtree_insert(tree, key, null);
    }
    benchmark::DoNotOptimize(tree);
    rbtree_free(tree);
  }
}

static void BM_GET(benchmark::State &state) {
  rbtree_t tree = null;
  for (val key : keys) {
    rbtree_insert(tree, key, null);
  }
  for (auto _ : state) {
    for (val key : keys) {
      auto value = rbtree_get(tree, key);
      benchmark::DoNotOptimize(value);
    }
  }
  rbtree_free(tree);
}

static void BM_DELETE(benchmark::State &state) {
  for (auto _ : state) {
    rbtree_t tree = null;
    for (val key : keys) {
      rbtree_insert(tree, key, null);
    }
    for (val key : keys) {
      rbtree_delete(tree, key);
    }
    benchmark::DoNotOptimize(tree);
  }
}

BENCHMARK(BM_INSERT);
BENCHMARK(BM_GET);
BENCHMARK(BM_DELETE);

BENCHMARK_MAIN();
