#include <version>
#if __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif
#if !defined(__cpp_lib_coroutine) || !defined(__cpp_concepts)
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

#include <chrono>
#include <exception>
#include <future>
#include <iostream>
#include <thread>
#include <type_traits>
//#endregion

// https://en.cppreference.com/w/cpp/coroutine/coroutine_traits

// Enable the use of std::future<T> as a coroutine type
// by using a std::promise<T> as the promise type.
template <typename T, typename... Args>
requires( !std::is_void_v<T> && !std::is_reference_v<T>) 
struct std::coroutine_traits<std::future<T>, Args...> {
  struct promise_type {
    std::promise<T> p;
    std::future<T> get_return_object() noexcept { return p.get_future(); }

    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }

    void return_value(const T &value) noexcept(
        std::is_nothrow_copy_constructible_v<T>) {
      p.set_value(value);
    }
    void return_value(T &&value) noexcept(std::is_nothrow_move_constructible_v<T>) {
      p.set_value(std::move(value));
    }
    void unhandled_exception() noexcept {
      p.set_exception(std::current_exception());
    }
  };
};

std::future<int> simple_return() {
  std::puts("Simple Return in progress");
  co_return 42;
}

// Same for std::future<void>.
template <typename... Args>
struct std::coroutine_traits<std::future<void>, Args...> {
  struct promise_type {
    std::promise<void> p;
    std::future<void> get_return_object() noexcept { return p.get_future(); }

    std::suspend_never initial_suspend() const noexcept { return {}; }
    std::suspend_never final_suspend() const noexcept { return {}; }

    void return_void() noexcept { p.set_value(); }
    void unhandled_exception() noexcept {
      p.set_exception(std::current_exception());
    }
  };
};

// Allow co_await'ing std::future<T> and std::future<void>
// by naively spawning a new thread for each co_await.
template <typename T>
auto operator co_await(std::future<T> future) noexcept
    requires(!std::is_reference_v<T>) {
  struct Awaitable : std::future<T> {
    bool await_ready() const noexcept {
      using namespace std::chrono_literals;
      return this->wait_for(0s) != std::future_status::timeout;
    }
    void await_suspend(std::coroutine_handle<> cont) const {
#ifdef __clang__
#define CLANG_MUTABLE \
  mutable // experimental/coroutines has not yet the right interface
#else
#define CLANG_MUTABLE
#endif
      std::thread([this, cont]() CLANG_MUTABLE {
        this->wait();
        cont.resume();
      }).detach();
    }
    T await_resume() { return this->get(); }
  };
  return Awaitable{std::move(future)};
}

// Utilize the infrastructure we have established.
std::future<int> compute() {
  int a = co_await std::async([] { return 6; });
  int b = co_await std::async([] { return 7; });
  co_return a * b;
}

std::future<void> fail() {
  throw std::runtime_error("Boom failure");
  co_return;
}

int main() {
  auto simple = simple_return();
  std::cout << simple.get() << '\n';
  
  std::cout << compute().get() << '\n';

  try {
    fail().get();
  } catch (const std::runtime_error &e) {
    std::cout << "error: " << e.what() << '\n';
  }
}

#endif