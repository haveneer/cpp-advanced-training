#include <version>
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

#include <coroutine>
#include <iostream>
#include <memory>
#include <optional>

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
    auto return_void() { return std::suspend_never{}; }

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

Generator<int> generate() noexcept {
  co_yield 1;
  co_yield 2;
  co_yield 3;
  co_yield 4;
}

int main() {
  auto g = generate();
  while (auto v = g.next()) {
    std::cout << v.value() << '\n';
  }
}

#endif