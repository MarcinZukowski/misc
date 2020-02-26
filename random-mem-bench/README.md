A simple random memory performance tester.

## Installation

Setup Google Benchmark
```
git clone https://github.com/google/benchmark.git
git clone https://github.com/google/googletest.git benchmark/googletest
cd benchmark
mkdir build && cd build
cmake -DCMAKE_INSTALL_PREFIX=../install -DCMAKE_BUILD_TYPE=Release ../
make -j 8
make install
cd ../..
```

Compile and run
```
mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ../
make -j 8
./random-mem-bench
```

## Custom options

Run `./random-mem-bench --help` to see all options, including both the custom ones
and the Google Benchmark built-in ones. 

Example custom options:
* `range_log2_min` - log2 of the memory range minimum, default `4` (=> `16 bytes`)
* `range_log2_min` - log2 of the memory range maximum, default `28` (=> `256MB`)
* `range_log2_step` - log2 of the step when growing used memory, default `3` (=> step=`8`)
* `threads_min` - min number of threads, default `1`
* `threads_max` - max number of threads  , default `1`

So, by default the benchmark will use a range of `16B` (`2**4`) to `256MB` (`2**28`), 
with a step of `8` (`2**3`), on a single thread.

To e.g. test memory ranges from 16 to 4GB, with up to 8 threads, run this
```
./random-mem-bench --range_log2_step=1 --range_log2_max=32 --threads_max=8
```


