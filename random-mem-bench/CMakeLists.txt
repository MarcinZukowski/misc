cmake_minimum_required(VERSION 3.14)
project(random-mem-bench)

set(CMAKE_CXX_STANDARD 14)

set(benchmark_prefix ${PROJECT_SOURCE_DIR}/benchmark/install/)
include(${benchmark_prefix}/lib64/cmake/benchmark/benchmarkConfig.cmake)

add_executable(random-mem-bench main.cpp)
target_link_libraries(random-mem-bench benchmark::benchmark)
