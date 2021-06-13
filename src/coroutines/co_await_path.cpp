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
#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#endif
#include <iostream>
#include <utility>
#ifndef _MSC_VER
#define PRINTME() std::cout << __PRETTY_FUNCTION__ << '\n'
#else
#define PRINTME() std::cout << __FUNCSIG__ << '\n'
#endif
//#endregion

struct CoResult {
  CoResult() {}
  CoResult(const CoResult &) { PRINTME(); }
  CoResult(CoResult &&) { PRINTME(); }
  void operator=(const CoResult &) { PRINTME(); }
  void operator=(CoResult &&) { PRINTME(); }
};

struct Awaitable {
  Awaitable() { PRINTME(); }
  ~Awaitable() { PRINTME(); }
  bool await_ready() const {
    PRINTME();
    return false;
  }
  auto await_suspend(std::coroutine_handle<>) const {
    // already suspended; you can do want you want with the handle
    PRINTME();
  }
  CoResult await_resume() const {
    // already resumed
    PRINTME();
    return {};
  }
};

struct Expr {
  Expr() { PRINTME(); }

  Awaitable operator co_await() {
    PRINTME();
    return {};
  }
};

class Task {
public:
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  Task(handle_type h) : m_co_handle(h) {}
  ~Task() {
    if (m_co_handle)
      m_co_handle.destroy();
  }
  Task(const Task &) = delete;
  Task(Task &&that) noexcept
      : m_co_handle(std::exchange(that.m_co_handle, nullptr)) {}
  Task &operator=(const Task &) = delete;
  void operator=(Task &&that) = delete;
  void resume() {
    PRINTME();
    m_co_handle.resume();
  }

  struct promise_type {
    promise_type() = default;
    ~promise_type() = default;

    std::suspend_never initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    auto get_return_object() { return Task{handle_type::from_promise(*this)}; }
    void return_void() {}
    void unhandled_exception() { std::exit(1); }

    [[nodiscard]] auto await_transform(const Expr &e) {
      PRINTME();
      return e;
    }
  };

  handle_type m_co_handle;
};

Task task() {
  CoResult r;
  std::puts("[Start task]");
  r = co_await Expr{}; // co_await result is built when the coroutine is resumed
  std::puts("[End task]");
}

int main() {
  std::puts("[Start main]");
  auto t = task();             // auto-start, will not stop before co_await
  std::puts("[Back to main]"); // can use task result in coroutine object
  t.resume();
  std::puts("[End main]");
}

#endif