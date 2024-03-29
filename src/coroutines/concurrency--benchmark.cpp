#include <version>
#if !defined(__cpp_lib_coroutine) && __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif
#ifndef __cpp_lib_coroutine
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_coroutine)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#endif
#include <benchmark/benchmark.h>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <numeric>
#include <optional>
#include <queue>
#include <random>
#include <thread>
//#endregion

struct Generator {
  struct promise_type {
    int m_val;
    Generator get_return_object() { return this; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { // always requires
      return {};                                   // by using m_handle.done()
    }                                              // to check the end (below)
    std::suspend_always yield_value(int val) {
      m_val = val;
      return {};
    }
    void return_void() {}
    void unhandled_exception() { std::exit(1); }
  };

  Generator(promise_type *p)
      : m_co_handle(std::coroutine_handle<promise_type>::from_promise(*p)) {}
  ~Generator() {
    if (m_co_handle)
      m_co_handle.destroy();
  }

  Generator(const Generator &) = delete;
  Generator &operator=(const Generator &) = delete;
  Generator(Generator &&that) noexcept
      : m_co_handle(std::exchange(that.m_co_handle, nullptr)) {}
  Generator &operator=(Generator &&that) noexcept {
    if (m_co_handle)
      m_co_handle.destroy();
    m_co_handle = std::exchange(that.m_co_handle, nullptr);
    return *this;
  }

  std::optional<int> next() { // undefined behavior if called after first nullopt
    m_co_handle.resume();     // Could be fixed by checking before 'resume' call
    if (not m_co_handle.done()) {
      return std::make_optional(m_co_handle.promise().m_val);
    } else {
      return std::nullopt;
    }
  }

  std::coroutine_handle<promise_type> m_co_handle;
};

constexpr unsigned int seed = std::mt19937::default_seed;
constexpr unsigned int size = 10; // weight of inner loop computation

static void start_cold_empty_thread(benchmark::State &state) {
  for (auto _ : state) {
    std::thread([] {}).detach();
  }
}
BENCHMARK(start_cold_empty_thread);

static void run_cold_thread(benchmark::State &state) {
  std::vector<int> v(size);
  std::mt19937 engine{seed};
  std::generate(std::begin(v), std::end(v), engine);
  for (auto _ : state) {
    std::thread th([&v] {
      auto data = std::accumulate(std::begin(v), std::end(v), 0);
      benchmark::DoNotOptimize(data);
    });
    th.join();
  }
}
BENCHMARK(run_cold_thread);

static void run_hot_thread(benchmark::State &state) {
  std::mutex queue_mutex;
  std::condition_variable queue_condition;
  std::queue<std::function<void()>> task_queue;
  bool stop = false;

  std::thread ready_thread([&] {
    while (true) {
      std::unique_lock lock(queue_mutex);
      queue_condition.wait(lock, [&] { return !task_queue.empty() || stop; });
      if (stop && task_queue.empty())
        return;
      std::function<void()> task = task_queue.front();
      task_queue.pop();
      lock.unlock();
      task();
    }
  });

  std::vector<int> v(size);
  std::mt19937 engine{seed};
  std::generate(std::begin(v), std::end(v), engine);
  bool keep_running = state.KeepRunning();
  while (keep_running) {
    std::unique_lock lock(queue_mutex);
    task_queue.push([&v] {
      auto data = std::accumulate(std::begin(v), std::end(v), 0);
      benchmark::DoNotOptimize(data);
    });
    lock.unlock();
    queue_condition.notify_one();

    keep_running = state.KeepRunning();
    if (!keep_running) { // Trick to count end of queue inside benchmark
      lock.lock();
      stop = true;
      lock.unlock();
      queue_condition.notify_all();
      ready_thread.join();
    }
  }
}
BENCHMARK(run_hot_thread);

static void run_function(benchmark::State &state) {
  std::vector<int> v(size);
  std::mt19937 engine{seed};
  std::generate(std::begin(v), std::end(v), engine);
  for (auto _ : state) {
    auto f = [&v] {
      auto data = std::accumulate(std::begin(v), std::end(v), 0);
      benchmark::DoNotOptimize(data);
    };
    f();
  }
}
BENCHMARK(run_function);

Generator coroutine(const std::vector<int> &v) {
  while (true) {
    auto data = std::accumulate(std::begin(v), std::end(v), 0);
    benchmark::DoNotOptimize(data);
    co_yield data;
  }
}

static void create_coroutine(benchmark::State &state) {
  std::vector<int> v(size);
  std::mt19937 engine{seed};
  std::generate(std::begin(v), std::end(v), engine);
  for (auto _ : state) {
    auto g = coroutine(v);
    benchmark::DoNotOptimize(g);
  }
}
BENCHMARK(create_coroutine);

static void next_call_coroutine(benchmark::State &state) {
  std::vector<int> v(size);
  std::mt19937 engine{seed};
  std::generate(std::begin(v), std::end(v), engine);
  auto g = coroutine(v);
  for (auto _ : state) {
    benchmark::DoNotOptimize(g.next());
  }
}
BENCHMARK(next_call_coroutine);

BENCHMARK_MAIN();

#endif