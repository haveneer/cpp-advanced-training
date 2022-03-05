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
#include <iostream>
#include <numeric>
#include <utility>
//#endregion

template <typename T> struct generator {
  struct promise_type {
    T current_value;
    std::suspend_always yield_value(T value) {
      this->current_value = value;
      return {};
    }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    generator get_return_object() { return generator{this}; };
    void unhandled_exception() { std::terminate(); }
    void return_void() {}
  };

  struct iterator {
    std::coroutine_handle<promise_type> _Coro;
    bool _Done;

    iterator(std::coroutine_handle<promise_type> Coro, bool Done)
        : _Coro(Coro), _Done(Done) {}

    iterator &operator++() {
      _Coro.resume();
      _Done = _Coro.done();
      return *this;
    }

    bool operator==(iterator const &_Right) const { return _Done == _Right._Done; }

    bool operator!=(iterator const &_Right) const { return !(*this == _Right); }
    T const &operator*() const { return _Coro.promise().current_value; }
    T const *operator->() const { return &(operator*()); }
  };

  iterator begin() {
    p.resume();
    return {p, p.done()};
  }

  iterator end() { return {p, true}; }

  generator(generator const &) = delete;
  generator(generator &&rhs) : p(rhs.p) { rhs.p = nullptr; }

  ~generator() {
    if (p)
      p.destroy();
  }

private:
  explicit generator(promise_type *p)
      : p(std::coroutine_handle<promise_type>::from_promise(*p)) {}

  std::coroutine_handle<promise_type> p;
};

template <typename T> generator<T> seq() {
  for (T i = {};; ++i)
    co_yield i;
}

template <typename T> generator<T> take_until(generator<T> &g, T limit) {
  for (auto &&v : g)
    if (v < limit)
      co_yield v;
    else
      break;
}

template <typename T> generator<T> multiply(generator<T> &g, T factor) {
  for (auto &&v : g)
    co_yield v *factor;
}

template <typename T> generator<T> add(generator<T> &g, T adder) {
  for (auto &&v : g)
    co_yield v + adder;
}

int main() {
  // The following sequence is completely inlined by Clang ≥ 10
  // A compiler avec les options: -O2 -std=c++20 --stdlib=libc++
  auto s = seq<int>();
  auto t = take_until(s, 10);
  auto m = multiply(t, 2);
  auto a = add(m, 110);
  return std::accumulate(a.begin(), a.end(), 0) - 166;
}

#endif