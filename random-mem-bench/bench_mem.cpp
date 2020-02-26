#include "bench_mem.hpp"

#include "utils.hpp"

/// Number of items per round
static constexpr int64_t PER_ROUND = 1'000'000;

void Bench::SetUp(const ::benchmark::State& st)
{
  if (st.thread_index > 0) {
    return;  // Only setup things in the 1st thread
  }
  // Allocate memory
  int64_t range = st.range(0);
  m_mem = reinterpret_cast<int64_t*>(malloc(range));
  if (m_mem == nullptr) {
    fprintf(stderr, "Couldn't allocate %lld bytes\n", range);
    exit(1);
  }
  m_count = range / sizeof(*m_mem);
  if (m_count <= 0) {
    fprintf(stderr, "Range too small\n");
    exit(1);
  }

  // Fill with random integers in the range
  srand(0);
  for (int64_t i = 0; i < m_count; ++i) {
    m_mem[i] = rand64() & (m_count - 1);
  }
}

void Bench::TearDown(const ::benchmark::State& st)
{
  if (st.thread_index > 0) {
    return;   // Only tear down in the 1st thread
  }
  free(m_mem);
}

void Bench_RandomWalk_Benchmark::BenchmarkCase(benchmark::State& state)
{
  for (auto _ : state) {
    int64_t idx = 0;
    int64_t offset = getThreadOffset(state.thread_index, m_count);
    for (int i = 0; i < PER_ROUND; ++i) {
      idx = (m_mem[idx] + i + offset) & (m_count - 1);
    }
    benchmark::DoNotOptimize(idx);
  }
  state.SetItemsProcessed(state.iterations() * PER_ROUND);
}

void Bench_RandomSum_Benchmark::BenchmarkCase(benchmark::State& state)
{
  for (auto _ : state) {
    int64_t sum = 0;
    int64_t offset = getThreadOffset(state.thread_index, m_count);
    for (int i = 0; i < PER_ROUND; ++i) {
      int64_t idx = (m_mem[(i + offset) & (m_count - 1)]);
      sum += m_mem[idx];
    }
    benchmark::DoNotOptimize(sum);
  }
  state.SetItemsProcessed(state.iterations() * PER_ROUND);
}
