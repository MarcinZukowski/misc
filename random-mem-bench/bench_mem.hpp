#pragma once
#ifndef RANDOM_MEM_BENCH_BENCH_MEM_HPP
#define RANDOM_MEM_BENCH_BENCH_MEM_HPP

#include "benchmark/benchmark.h"

class BenchMem : public ::benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& st) override;
  void TearDown(const ::benchmark::State& st) override;

  int64_t m_count;
  int64_t *m_mem;
};

BENCHMARK_PRIVATE_DECLARE_F(BenchMem, RandomWalk);
BENCHMARK_PRIVATE_DECLARE_F(BenchMem, RandomSum);

#endif //RANDOM_MEM_BENCH_BENCH_MEM_HPP
