//#region [Collapse all]
#include <atomic>
#include <benchmark/benchmark.h>
#include <cassert>
#include <iostream>
#include <mutex>
#include <thread>
//#endregion

const unsigned count = 100'000;

unsigned naive_test() {
  unsigned data{};
  auto f = [&]() {
    for (unsigned i = count; i > 0; --i) {
      ++data;
      benchmark::DoNotOptimize(data);
    }
  };
  std::thread t1(f);
  std::thread t2(f);

  t1.join();
  t2.join();
  return data;
}

unsigned mutex_test() {
  std::mutex mutex;
  unsigned data{};
  auto f = [&]() {
    for (unsigned i = count; i > 0; --i) {
      std::lock_guard<std::mutex> lock(mutex);
      ++data;
      benchmark::DoNotOptimize(data);
    }
  };
  std::thread t1(f);
  std::thread t2(f);

  t1.join();
  t2.join();
  return data;
}

unsigned atomic_test() {
  std::atomic<unsigned> data{};
  assert(data.is_lock_free());
  auto f = [&]() {
    for (unsigned i = count; i > 0; --i) {
      ++data; // equivalent to: data.fetch_add(1, std::memory_order::seq_cst);
      // data.fetch_add(1, std::memory_order::relaxed); // FIXME faster but  /!\ ...
      benchmark::DoNotOptimize(data);
    }
  };
  std::thread t1(f);
  std::thread t2(f);

  t1.join();
  t2.join();
  return data;
}

unsigned empty_test() {
  unsigned data{};
  auto f = [&]() {
    for (unsigned i = count; i > 0; --i) {
      benchmark::DoNotOptimize(data);
    }
  };
  std::thread t1(f);
  std::thread t2(f);

  t1.join();
  t2.join();
  return data;
}

static void naive(benchmark::State &state) {
  for (auto _ : state) {
    auto data = naive_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(naive);

static void mutex(benchmark::State &state) {
  for (auto _ : state) {
    auto data = mutex_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(mutex);

static void atomic(benchmark::State &state) {
  for (auto _ : state) {
    auto data = atomic_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(atomic);

static void empty(benchmark::State &state) {
  for (auto _ : state) {
    auto data = empty_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(empty);

int main(int argc, char **argv) {
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;

  auto naive_data = naive_test();
  std::cout << "naive result data = " << naive_data << std::endl;
  // assert(data == 2*count); // TODO What is the value of 'data' ?
  auto mutex_data = mutex_test();
  std::cout << "mutex result data = " << mutex_data << std::endl;
  assert(mutex_data == 2 * count); // TODO What is the value of 'data' ?
  auto atomic_data = atomic_test();
  std::cout << "atomic result data = " << atomic_data << std::endl;
  assert(atomic_data == 2 * count); // TODO What is the value of 'data' ?

  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
}