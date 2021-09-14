#include <version>
#if !defined(__cpp_lib_memory_resource)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_memory_resource)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <benchmark/benchmark.h>

// Special memory management :
// https://en.cppreference.com/w/cpp/header/memory_resource
// see also:
// https://stackoverflow.com/questions/38010544/polymorphic-allocator-when-and-why-should-i-use-it
// https://www.bfilipek.com/2020/08/pmr-dbg.html

// Copy following part into http://quick-bench.com
// http://quick-bench.com/IBMiNRYcqI3N82mJdeTYu9pPgho

// %<------------------------------------------------

#include <list>
#include <memory_resource>
#include <vector>

static void PMRList(benchmark::State &state) {
  for (auto _ : state) {
    std::byte stackBuf[2048];
    std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
    std::pmr::list<int> listOfThings{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        &rsrc};
    benchmark::DoNotOptimize(listOfThings);
  }
}

BENCHMARK(PMRList);

static void PMRVector(benchmark::State &state) {
  for (auto _ : state) {
    std::byte stackBuf[2048];
    std::pmr::monotonic_buffer_resource rsrc(stackBuf, sizeof stackBuf);
    std::pmr::vector<int> listOfThings{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19},
        &rsrc};
    benchmark::DoNotOptimize(listOfThings);
  }
}

BENCHMARK(PMRVector);

static void standardList(benchmark::State &state) {
  for (auto _ : state) {
    std::list<int> listOfThings{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}};
    benchmark::DoNotOptimize(listOfThings);
  }
}

BENCHMARK(standardList);

static void standardVector(benchmark::State &state) {
  for (auto _ : state) {
    std::vector<int> listOfThings{
        {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19}};
    benchmark::DoNotOptimize(listOfThings);
  }
}

BENCHMARK(standardVector);

// ------------------------------------------------>%

BENCHMARK_MAIN();

#endif