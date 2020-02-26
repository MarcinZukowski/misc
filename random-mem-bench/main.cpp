#include "benchmark/benchmark.h"

#include <cstdlib>
#include <cstring>

long rand64()
{
  return (static_cast<int64_t>(rand()) << 32) | rand();
}

/// Number of items per round
static constexpr int64_t PER_ROUND = 1'000'000;

// Various options configurable via CLI
static int64_t RANGE_LOG2_MIN = 4;
static int64_t RANGE_LOG2_MAX = 28;
static int64_t RANGE_LOG2_STEP = 3;
static int64_t THREADS_MIN = 1;
static int64_t THREADS_MAX = 1;

// Are we in "--help" mode ?
static bool IN_HELP = false;

// Common setup
class Bench : public ::benchmark::Fixture {
public:
  void SetUp(const ::benchmark::State& st) override
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

  void TearDown(const ::benchmark::State& st) override
  {
    if (st.thread_index > 0) {
      return;   // Only tear down in the 1st thread
    }
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

/**
 * Handle a single custom option.
 * @return true if the particular option was detected and successfully parsed.
 *         target is modified in this case
 */
bool parse_flag_int64(char *arg, const char *option, int64_t *target)
{
  size_t optLen = strlen(option);
  if (strlen(arg) <= optLen + 3)
    return false;  // too short
  if (strncmp(arg, "--", 2) != 0)
    return false;  // need "--"
  arg += 2;
  if (strncmp(arg, option, optLen) != 0)
    return false;  // doesn't match
  if (arg[optLen ] != '=')
    return false;  // no '='
  char *endPtr;
  long long val = strtoll(arg + optLen + 1, &endPtr, 10);
  if (*endPtr != '\0')
    return false;  // not all parsed
  *target = val;
  return true;  // we're good
}

/// A single CLI option
struct option
{
  const char *name;
  int64_t *valPtr;
};

std::vector<option> options {
    {"range_log2_min", &RANGE_LOG2_MIN},
    {"range_log2_max", &RANGE_LOG2_MAX},
    {"range_log2_step", &RANGE_LOG2_STEP},
    {"threads_min", &THREADS_MIN},
    {"threads_max", &THREADS_MAX},
};

/**
 * Handle custom options. Returns unknown options, these will be passed to
 * the default benchmark handler
 */
std::vector<char*> handle_custom_options(int argc, char** argv)
{
  std::vector<char*> remaining;
  for (int i = 0; i < argc; i++) {
    char* arg = argv[i];
    bool found = false;
    for (auto &o: options) {
      if (parse_flag_int64(arg, o.name, o.valPtr)) {
        found = true;
        break;
      }
    }
    if (strcmp(arg, "--help") == 0) {
      fprintf(stderr, "\nCustom options:\n");
      for (auto &o: options) {
        fprintf(stderr, "    --%s=<int>\n", o.name);
      }
      fprintf(stderr, "\n");
      IN_HELP = true;
    }
    if (!found) {
      // We don't know this option, pass it on
      remaining.push_back(arg);
    }
  }
  return remaining;
}

/**
 * Handle common options for all benchmarks
 */
void register_benchmarks()
{
  if (IN_HELP)
    return;  // Don't register if in "--help"

  fprintf(stderr, "Using custom settings:\n");
  for (auto o: options) {
    fprintf(stderr, "    %-15s : %lld\n", o.name, *o.valPtr);
  }

  // Known benchmarks
  std::vector<benchmark::internal::Benchmark*> benchmarks{
      new Bench_RandomSum_Benchmark(),
      new Bench_RandomWalk_Benchmark(),
  };

  for (auto &b: benchmarks) {
    b = ::benchmark::internal::RegisterBenchmarkInternal(b);
    b = b->RangeMultiplier(1LL << RANGE_LOG2_STEP);
    b = b->Range(1LL << RANGE_LOG2_MIN, 1LL << RANGE_LOG2_MAX);
    if (THREADS_MAX > 1) {
      b = b->ThreadRange(THREADS_MIN, THREADS_MAX);
    }
  }
}

int main(int argc, char** argv)
{
  std::vector<char*> remaining_options = handle_custom_options(argc, argv);
  argc = remaining_options.size();
  argv = remaining_options.data();

  register_benchmarks();

  benchmark::Initialize(&argc, argv);
  if (benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  benchmark::RunSpecifiedBenchmarks();
}
