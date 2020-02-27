#include "bench_alloc.hpp"

#include <algorithm>
#include <cstring>

template<bool genRandomSizes>
void BenchAlloc<genRandomSizes>::SetUp(const ::benchmark::State& st)
{
  if (st.thread_index == 0) {
    // Only setup things in the 1st thread
    m_size = st.range(0);
    m_count = std::min(MAX_ALLOC_MEMORY / m_size, MAX_ALLOCATIONS);

    // Create per-thread vectors
    m_allocations.resize(st.threads);
    m_sizes.resize(st.threads);
  }

  srand(st.thread_index);
  std::vector<size_t> &mySizes = m_sizes[st.thread_index];
  assert(mySizes.empty());
  for (int64_t i = 0; i < m_count; i++) {
    if constexpr (genRandomSizes) {
      // Create multiple sizes from a fraction of m_size to total m_size
      // TODO: do something better here
      int64_t div = std::min(m_size, m_count);
      size_t sz = m_size / div * ((i % div) + 1);
      assert(sz);
      size_t cnt = m_size / sz;
      for (int64_t s = 0; s < cnt; s++) {
        mySizes.push_back(rand() % sz + 4);
      }
    } else {
      mySizes.push_back(m_size);
    }
  }

  if constexpr (genRandomSizes) {
    // Shuffle allocation sizes
    std::random_shuffle(mySizes.begin(), mySizes.end());
  }
}

template<bool genRandomSizes>
void BenchAlloc<genRandomSizes>::TearDown(const ::benchmark::State& st)
{
  for (auto &v: m_sizes) {
    v.clear();
  }
}

template<bool genRandomSizes>
template<bool doMemset>
void BenchAlloc<genRandomSizes>::malloc_helper(benchmark::State& st)
{
  std::vector<void*> &myAllocs = m_allocations[st.thread_index];
  std::vector<size_t> &mySizes = m_sizes[st.thread_index];
  int64_t total = 0;
  for (auto _ : st) {

    assert(myAllocs.empty());
    for (auto sz: mySizes) {
      total += sz;
      void *ptr = malloc(sz);
      if constexpr (doMemset) {
        memset(ptr, 0x77, sz);
      }
      myAllocs.push_back(ptr);
    }

    for (auto ptr: myAllocs) {
      free(ptr);
    }
    myAllocs.clear();
  }

  st.SetItemsProcessed(st.iterations() * mySizes.size());
  st.counters["bytes_per_sec"] = benchmark::Counter(total, benchmark::Counter::kIsRate);
  st.counters["allocs_per_iter"] = mySizes.size();
}

void BenchAlloc_Malloc_Benchmark::BenchmarkCase(benchmark::State& st)
{
  malloc_helper<false>(st);
}

void BenchAlloc_MallocAndMemset_Benchmark::BenchmarkCase(benchmark::State& st)
{
  malloc_helper<true>(st);
}

void BenchAlloc_RandomMalloc_Benchmark::BenchmarkCase(benchmark::State& st)
{
  malloc_helper<false>(st);
}

void BenchAlloc_RandomMallocAndMemset_Benchmark::BenchmarkCase(benchmark::State& st)
{
  malloc_helper<true>(st);
}

