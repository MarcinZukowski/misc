cmake_minimum_required(VERSION 3.14)
project(random_mem_bench)

set(CMAKE_CXX_STANDARD 14)

set(benchmark_prefix ${PROJECT_SOURCE_DIR}/benchmark/install/)
include(${benchmark_prefix}/lib64/cmake/benchmark/benchmarkConfig.cmake)

add_executable(random_mem_bench main.cpp)
target_link_libraries(random_mem_bench benchmark::benchmark)
