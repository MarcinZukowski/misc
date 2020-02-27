#pragma once
#ifndef RANDOM_MEM_BENCH_BENCH_ALLOC_HPP
#define RANDOM_MEM_BENCH_BENCH_ALLOC_HPP

#include "benchmark/benchmark.h"

static constexpr int64_t MAX_ALLOC_MEMORY = 1 << 29;  // per thread per run
static constexpr int64_t MAX_ALLOCATIONS = 1 << 16;  // allocations max per run

template <bool genRandomSizes=false>
class BenchAlloc : public ::benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& st) override;
  void TearDown(const ::benchmark::State& st) override;

  int64_t m_count;
  int64_t m_size;

  // Per-thread vectors of sizes to allocate
  std::vector<std::vector<size_t>> m_sizes;

  // Per-thread vector of allocations so far
  std::vector<std::vector<void*>> m_allocations;

protected:
  template <bool doMemset>
  void malloc_helper(benchmark::State& st);
};

BENCHMARK_TEMPLATE_PRIVATE_DECLARE_F(BenchAlloc, Malloc);
BENCHMARK_TEMPLATE_PRIVATE_DECLARE_F(BenchAlloc, MallocAndMemset);
BENCHMARK_TEMPLATE_PRIVATE_DECLARE_F(BenchAlloc, RandomMalloc, true/*genRandomSizes*/);
BENCHMARK_TEMPLATE_PRIVATE_DECLARE_F(BenchAlloc, RandomMallocAndMemset, true/*genRandomSizes*/);

#endif //RANDOM_MEM_BENCH_BENCH_ALLOC_HPP
