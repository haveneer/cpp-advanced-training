//#region [Collapse all]
#include <atomic>
#include <benchmark/benchmark.h>
#include <condition_variable>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <thread>
//#endregion

const int count_limit = 100'000;

unsigned atomic_bool_test() {
  std::atomic<bool> atomicBool{};
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) { // move forward if false
      bool expected = false;
      while (!atomicBool.compare_exchange_strong(expected, true))
        ;
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) { // move forward if true
      bool expected = true;
      while (!atomicBool.compare_exchange_strong(expected, false))
        ;
      ++counter;
    }
  });
  ping.join();
  pong.join();
  return counter;
}

static void atomic_bool(benchmark::State &state) {
  for (auto _ : state) {
    auto data = atomic_bool_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(atomic_bool);

#ifdef my_cpp_lib_atomic_wait
unsigned atomic_wait_test() {
  std::atomic_flag condAtomicFlag{};
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) { // false -> true
      condAtomicFlag.wait(true);     // wait for != true
      // std::cout << "Ping: " << counter << std::endl;
      condAtomicFlag.test_and_set();
      condAtomicFlag.notify_one();
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) { // true -> false
      condAtomicFlag.wait(false);    // wait for != false
      // std::cout << "Pong: " << counter << std::endl;
      ++counter;
      condAtomicFlag.clear();
      condAtomicFlag.notify_one();
    }
  });
  ping.join();
  pong.join();
  return counter;
}

static void atomic_wait(benchmark::State &state) {
  for (auto _ : state) {
    auto data = atomic_wait_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(atomic_wait);
#endif

#ifdef my_cpp_lib_atomic_wait
unsigned atomic_flags_test() {
  std::atomic_flag condAtomicFlag1{};
  std::atomic_flag condAtomicFlag2{};
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) { // false -> true
      condAtomicFlag1.wait(false);   // wait for != false
      condAtomicFlag1.clear();
      // std::cout << "Ping " << counter << std::endl;
      condAtomicFlag2.test_and_set();
      condAtomicFlag2.notify_one();
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) { // true -> false
      condAtomicFlag2.wait(false);   // wait for != true
      condAtomicFlag2.clear();
      // std::cout << "Pong " << counter << std::endl;
      ++counter;
      condAtomicFlag1.test_and_set();
      condAtomicFlag1.notify_one();
    }
  });
  ping.join();
  pong.join();
  return counter;
}

static void atomic_flags(benchmark::State &state) {
  for (auto _ : state) {
    auto data = atomic_flags_test();
    benchmark::DoNotOptimize(data);
  }
}
// BENCHMARK(atomic_flags); // BUG ?
#endif

unsigned condition_variable_test() {
  bool dataReady{false};
  std::mutex mutex_;
  std::condition_variable condVar1;
  std::condition_variable condVar2;
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) {
      {
        std::unique_lock<std::mutex> lck(mutex_);
        condVar1.wait(lck, [&] { return dataReady == false; });
        dataReady = true;
      }
      condVar2.notify_one();
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) {
      {
        std::unique_lock<std::mutex> lck(mutex_);
        condVar2.wait(lck, [&] { return dataReady == true; });
        dataReady = false;
      }
      ++counter;
      condVar1.notify_one();
    }
  });
  ping.join();
  pong.join();
  return counter;
}

static void condition_variable(benchmark::State &state) {
  for (auto _ : state) {
    auto data = condition_variable_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(condition_variable);

#ifdef __cpp_lib_semaphore
#include <semaphore>

unsigned counting_semaphore_test() {
  std::counting_semaphore<1> signal_to_ping(0); // should be a std::binary_semaphore
  std::counting_semaphore<1> signal_to_pong(0); // 0 => blocks all first .acquire()
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) {
      signal_to_ping.acquire();
      // std::osyncstream(std::cout) << "Ping " << counter << std::endl;
      signal_to_pong.release();
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) {
      signal_to_pong.acquire();
      // std::osyncstream(std::cout) << "Pong " << counter << std::endl;
      ++counter;
      signal_to_ping.release();
    }
  });
  signal_to_ping.release();
  ping.join();
  pong.join();
  return counter;
}

static void counting_semaphore(benchmark::State &state) {
  for (auto _ : state) {
    auto data = counting_semaphore_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(counting_semaphore);

unsigned binary_semaphore_test() {
  std::binary_semaphore signal_to_ping(0);
  std::binary_semaphore signal_to_pong(0);
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) {
      signal_to_ping.acquire();
      // std::osyncstream(std::cout) << "Ping " << counter << std::endl;
      signal_to_pong.release();
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) {
      signal_to_pong.acquire();
      // std::osyncstream(std::cout) << "Pong " << counter << std::endl;
      ++counter;
      signal_to_ping.release();
    }
  });
  signal_to_ping.release();
  ping.join();
  pong.join();
  return counter;
}

static void binary_semaphore(benchmark::State &state) {
  for (auto _ : state) {
    auto data = binary_semaphore_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(binary_semaphore);
#endif

unsigned empty_test() {
  std::atomic<int> counter{};
  std::thread ping([&] {
    while (counter <= count_limit) {
    }
  });
  std::thread pong([&] {
    while (counter <= count_limit) {
      ++counter;
    }
  });
  ping.join();
  pong.join();
  return counter;
}

static void empty(benchmark::State &state) {
  for (auto _ : state) {
    auto data = empty_test();
    benchmark::DoNotOptimize(data);
  }
}
BENCHMARK(empty);

template <typename F> void timeit(std::string_view name, F &&f) {
  auto start = std::chrono::system_clock::now();
  f();
  std::chrono::duration<double> dur = std::chrono::system_clock::now() - start;
  std::cout << std::setw(20) << name << ": " << dur.count() << " seconds" << '\n';
}

int main(int argc, char **argv) {
  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;

  timeit("atomic<bool>", atomic_bool_test);
#ifdef my_cpp_lib_atomic_wait
  timeit("atomic_wait", atomic_wait_test);
#endif
  timeit("condition_variable", condition_variable_test);
#ifdef __cpp_lib_semaphore
  timeit("counting_semaphore", counting_semaphore_test);
  timeit("binary_semaphore", binary_semaphore_test);
#endif
  timeit("empty", empty_test);
  
  ::benchmark::RunSpecifiedBenchmarks();
  ::benchmark::Shutdown();
}