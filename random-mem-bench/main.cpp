#include "benchmark/benchmark.h"

#include <cstdlib>
#include <cstring>

#include "bench_alloc.hpp"
#include "bench_mem.hpp"

// Various options configurable via CLI
static int64_t RANGE_LOG2_MIN = 4;
static int64_t RANGE_LOG2_MAX = 28;
static int64_t RANGE_LOG2_STEP = 3;
static int64_t THREADS_MIN = 1;
static int64_t THREADS_MAX = 1;
static int64_t USE_REAL_TIME = 0;

// Are we in "--help" mode ?
static bool IN_HELP = false;

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
    {"use_real_time", &USE_REAL_TIME},
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
      new BenchMem_RandomSum_Benchmark(),
      new BenchMem_RandomWalk_Benchmark(),
      new BenchAlloc_Malloc_Benchmark(),
      new BenchAlloc_MallocAndMemset_Benchmark(),
      new BenchAlloc_RandomMalloc_Benchmark(),
      new BenchAlloc_RandomMallocAndMemset_Benchmark(),
  };

  for (auto &b: benchmarks) {
    b = ::benchmark::internal::RegisterBenchmarkInternal(b);
    b = b->RangeMultiplier(1LL << RANGE_LOG2_STEP);
    b = b->Range(1LL << RANGE_LOG2_MIN, 1LL << RANGE_LOG2_MAX);
    if (THREADS_MAX > 1) {
      b = b->ThreadRange(THREADS_MIN, THREADS_MAX);
    }
    if (USE_REAL_TIME) {
      b = b->UseRealTime();
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
