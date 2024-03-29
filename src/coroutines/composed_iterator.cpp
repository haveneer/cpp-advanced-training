#include <version>
#if !defined(__cpp_lib_coroutine) && __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif

#if !defined(__cpp_concepts) || !defined(__cpp_lib_coroutine)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_coroutine)});
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
#include <list>
#include <set>
#include <vector>
//#endregion

struct Generator {
  struct promise_type {
    int m_val;
    Generator get_return_object() { return this; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { // std::suspend_always requires
      return {};                                   // by using m_handle.done()
    }                                              // to check the end (below)
    std::suspend_always yield_value(int val) {
      m_val = val;
      return {};
    }
    void return_void() {}
    void unhandled_exception() { std::exit(1); }
  };
  /* -------- Iterator Implementation ---------- */
  struct iterator {
    bool operator!=(const iterator &rhs) const { return !m_h_ptr->done(); }
    iterator &operator++() {
      m_h_ptr->resume();
      return *this;
    }
    int operator*() const { return m_h_ptr->promise().m_val; }
    std::coroutine_handle<promise_type> *m_h_ptr;
  };
  iterator begin() { return iterator{&m_co_handle}; }
  iterator end() { return iterator{nullptr}; } // to only satisfy for-range API
  /* -------------------------------------------- */
  Generator(promise_type *p)
      : m_co_handle(std::coroutine_handle<promise_type>::from_promise(*p)) {}
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
    if (m_co_handle)
      m_co_handle.destroy();
    m_co_handle = that.m_co_handle;
    that.m_co_handle = nullptr;
    return *this;
  }

  std::coroutine_handle<promise_type> m_co_handle;
};

template <typename C>
concept Container = requires {
  typename C::value_type;
};

template <Container C> Generator flatten_iterate(const C &c) {
  if constexpr (Container<typename C::value_type>) {
    for (auto &&outer : c) {
      for (auto &&inner : flatten_iterate(outer)) {
        co_yield inner;
      }
    }
  } else {
    for (auto &&x : c) {
      co_yield x;
    }
  }
}

int main() {
  std::list<std::vector<std::set<int>>> lv{{{1, 2}, {3, 4}}, {{5, 6}, {7, 8}}};
  for (auto i : flatten_iterate(lv)) {
    std::cout << i << ' ';
  }
  std::cout << "\n";
}

#endif