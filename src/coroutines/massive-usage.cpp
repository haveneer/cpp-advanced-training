#include <version>
#if __has_include(<experimental/coroutine>)
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
#if __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#else
#include <coroutine>
#endif
#include <array>
#include <iostream>
#include <optional>
#include <vector>
//#endregion

size_t allocation_count = 0; // should be atomic if concurrent access

void *operator new(std::size_t n) {
  allocation_count += n;
  return malloc(n);
}

template <typename T> class Generator {
public:
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  Generator(handle_type h) : m_co_handle(h) {} // (3)

  ~Generator() {
    if (m_co_handle)
      m_co_handle.destroy();
  }
  Generator(const Generator &) = delete;
  Generator &operator=(const Generator &) = delete;
  Generator(Generator &&that) noexcept : m_co_handle(that.m_co_handle) {
    that.m_co_handle = nullptr;
  }
  Generator &operator=(Generator &&that) noexcept {
    m_co_handle = that.m_co_handle;
    that.m_co_handle = nullptr;
    return *this;
  }

  std::optional<T> next() { // (5)
    m_co_handle.resume();
    if (not m_co_handle.done()) {
      return std::make_optional(m_co_handle.promise().current_value);
    } else {
      return std::nullopt;
    }
  }

  struct promise_type {
    promise_type() = default; // (1)
    ~promise_type() = default;

    auto initial_suspend() { // (4)
      return std::suspend_always{};
    }
    auto final_suspend() noexcept { return std::suspend_always{}; }
    auto get_return_object() { // (2)
      return Generator{handle_type::from_promise(*this)};
    }
    void return_void() {}

    auto yield_value(const T value) { // (6)
      current_value = value;
      return std::suspend_always{};
    }
    void unhandled_exception() { std::exit(1); }
    T current_value;
  };

private:
  handle_type m_co_handle;
};

Generator<int> generate(int n) noexcept {
  // std::array<int, 0> v; // simulate more data in coroutine
  while (true) {
    co_yield n;
    if (n & 1) {
      n = 3 * n + 1;
    } else {
      n /= 2;
    }
  }
}

#include <chrono>

int main() {
  std::cout << "sizeof(promise_type) = " << sizeof(Generator<int>::promise_type)
            << "\n";
  std::cout << "sizeof(Generator<int>) = " << sizeof(Generator<int>) << "\n";

  const size_t co_count = 100'000;
  std::vector<Generator<int>> coroutines;
  coroutines.reserve(co_count);
  allocation_count = 0; // reset allocation count after Generator storage
  for (size_t i = 0; i < co_count; ++i) {
    coroutines.emplace_back(generate(i));
  }

  auto start = std::chrono::steady_clock::now();
  const uint64_t iter_count = 10;
  for (auto countdown = iter_count; countdown-- > 0;)
    for (auto &c : coroutines) {
      c.next();
    }
  auto end = std::chrono::steady_clock::now();
  std::chrono::duration<double, std::nano> elapsed = end - start;
  std::cout << "Heap allocation using new: " << allocation_count / co_count
            << " bytes / coroutine (includes promise; don't include generator)\n";
  std::cout << "Runs in " << elapsed.count() / co_count / iter_count
            << " ns / coroutine call" << '\n';
}

#endif