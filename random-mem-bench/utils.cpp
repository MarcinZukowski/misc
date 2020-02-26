#include "utils.hpp"

int64_t getThreadOffset(int threadNum, int64_t range)
{
  // Some non-cyclic fraction of the range
  int64_t idx = (range * threadNum * 13337 / 31337) % range;
  return idx;
}
