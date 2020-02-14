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
