# Point Cloud Processing Toolkit

| Windows | Ubuntu | MacOS |
| --- | --- | --- |
| [![Build status](https://ci.appveyor.com/api/projects/status/vlci0d4xfeo0p4y9/branch/master?svg=true)](https://ci.appveyor.com/project/Q-Minh/point-cloud-processing/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/vto8v29cpp5v9jrt/branch/master?svg=true)](https://ci.appveyor.com/project/Q-Minh/point-cloud-processing-05x2j/branch/master) | [![Build status](https://ci.appveyor.com/api/projects/status/hnb1af22xdu51vtv/branch/master?svg=true)](https://ci.appveyor.com/project/Q-Minh/point-cloud-processing-m4p2m/branch/master) |
   
[![codecov](https://codecov.io/gh/Q-Minh/point-cloud-processing/branch/master/graph/badge.svg?token=ICLU539TV2)](https://codecov.io/gh/Q-Minh/point-cloud-processing) 
[![License](https://img.shields.io/badge/License-Boost%201.0-lightblue.svg)](https://www.boost.org/LICENSE_1_0.txt)

## Overview

`pcp` is a toolkit of common point cloud processing algorithms.

## Prerequisites

- [CMake](https://cmake.org/)
- [C++17](https://en.cppreference.com/w/cpp/17)

## Configuring

| CMake Option | Help |
| --- | --- |
| `-DPCP_BUILD_TESTS` | If set to `ON`, downloads [Catch2](https://github.com/catchorg/Catch2) and builds the `tests` target |
| `-DPCP_BUILD_BENCHMARKS` | If set to `ON`, downloads [Google Benchmark](https://github.com/google/benchmark) and builds the `benchmarks` target |
| `-DPCP_BUILD_EXAMPLES` | If set to `ON`, builds the examples. |
| `-DCMAKE_BUILD_TYPE` | Set to one of `Release\|Debug\|RelWithDebInfo`. If you are using a Visual Studio [generator](https://cmake.org/cmake/help/latest/manual/cmake-generators.7.html), all configuration types are included. See the [docs](https://cmake.org/cmake/help/latest/variable/CMAKE_BUILD_TYPE.html). |  
  
```
$ cd <path to repo>
$ mkdir build
$ cd build
$ cmake .. -DPCP_BUILD_TESTS=ON -DPCP_BUILD_BENCHMARKS=ON
```

## Building

| CMake Target | Help |
| --- | --- |
| `pcp-tests` | The [Catch2](https://github.com/catchorg/Catch2) test runner executable. Commands line options are documented [here](https://github.com/catchorg/Catch2/blob/devel/docs/command-line.md#top). |
| `pcp-benchmarks` | The [Google Benchmark](https://github.com/google/benchmark) benchmark runner executable. Command line options are documented [here](https://github.com/google/benchmark#command-line). |
| `pcp` | The `pcp` library. Currently header-only, so building is a no-op. |
| `pcp-example-ply` | Ply read/write example. |
| `pcp-example-normals-estimation` | Reads a ply point cloud and exports it back with normals. |
```
$ cd <path to repo>
$ cd build
$ cmake --build . --target <target to build>
```

## Installing
```
$ cd <path to repo>
$ cd build
$ cmake --install .
```

## Usage
In your CMake project's CMakeLists.txt:
```
find_package(pcp CONFIG REQUIRED)
target_link_library(<your target> PRIVATE pcp::pcp)
```

Single include:
```
#include <pcp.hpp>
```

Multiple includes:
```
#include <pcp/octree.hpp>
#include <pcp/traits/graph_traits.hpp>
#include <pcp/graph/directed_adjacency_list.hpp>
```

Find code usage examples [here](./examples/).  
Find the tests [here](./test/).

## Running
Run tests, benchmarks and visualize benchmark results:
```
# In the build tree, the executables will either be in "./build/Release/" or "./build/" depending on the platform.
# In the install tree, you can find the executables in "/bin".

# to run the tests
$ <path to executable>/pcp-tests.exe

# to run the benchmarks and export them to benchmarks.json
$ <path to executable>/pcp-benchmarks.exe --benchmark_format=json --benchmark_out=benchmarks.json
```

## Documentation

See [docs](./doc/)
