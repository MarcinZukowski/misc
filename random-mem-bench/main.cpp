#include "benchmark/benchmark.h"

#include <cstdlib>

long rand64()
{
  return (static_cast<int64_t>(rand()) << 32) | rand();
}

static constexpr int64_t PER_ROUND = 1'000'000;

class Bench : public ::benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& st) override
  {
    // Allocate memory
    int64_t range = st.range(0);
    m_mem = reinterpret_cast<int64_t*>(malloc(range));
    m_count = range / sizeof(*m_mem);

    // Fill with random integers in the range
    srand(0);
    for (int64_t i = 0; i < m_count; ++i) {
      m_mem[i] = rand64() & (m_count - 1);
    }
  }

  void TearDown(const ::benchmark::State&) override
  {
    free(m_mem);
  }

  int64_t m_count;
  int64_t *m_mem;
};

BENCHMARK_DEFINE_F(Bench, RandomWalk)(benchmark::State& state)
{
  for (auto _ : state) {
    int64_t idx = 0;
    for (int i = 0; i < PER_ROUND; ++i) {
      idx = (m_mem[idx] + i) & (m_count - 1);
    }
    benchmark::DoNotOptimize(idx);
  }
  state.SetItemsProcessed(state.iterations() * PER_ROUND);
}
BENCHMARK_REGISTER_F(Bench, RandomWalk)->Range(1 << 4, 1 << 28);

BENCHMARK_DEFINE_F(Bench, RandomSum)(benchmark::State& state)
{
  for (auto _ : state) {
    int64_t sum = 0;
    for (int i = 0; i < PER_ROUND; ++i) {
      int64_t idx = (m_mem[i & (m_count - 1)]);
      sum += m_mem[idx];
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetItemsProcessed(state.iterations() * PER_ROUND);
}
BENCHMARK_REGISTER_F(Bench, RandomSum)->Range(1 << 4, 1 << 28);

BENCHMARK_MAIN();
