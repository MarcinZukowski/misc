#pragma once
#ifndef RANDOM_MEM_BENCH_UTILS_HPP
#define RANDOM_MEM_BENCH_UTILS_HPP

#include <stdlib.h>

static inline int64_t rand64()
{
  return (static_cast<int64_t>(rand()) << 32) | rand();
}

int64_t getThreadOffset(int threadNum, int64_t range);

#endif //RANDOM_MEM_BENCH_UTILS_HPP
