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
#include <optional>
//#endregion

struct Awaitable {
  Awaitable(const char *name) : name(name) {}
  bool await_ready() const { return false; }
  void await_suspend(std::coroutine_handle<>) const {}
  auto await_resume() const { return name; }
  const char *name;
};

struct Expr1 {
  auto operator co_await() {
    std::puts("Expr1->Awaitable using member operator");
    return Awaitable{"from Expr1"};
  }
};

struct Expr2 {};
auto operator co_await(Expr2 &&) {
  std::puts("Expr2->Awaitable using free operator");
  return Awaitable{"from Expr2"};
}

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
  Task &operator=(const Task &) = delete;
  Task(Task &&that) noexcept
      : m_co_handle(std::exchange(that.m_co_handle, nullptr)) {}
  void operator=(Task &&that) noexcept {
    m_co_handle = std::exchange(that.m_co_handle, nullptr);
  }
  void next() { m_co_handle.resume(); }

  struct promise_type {
    promise_type() = default;
    ~promise_type() = default;

    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
    auto get_return_object() { return Task{handle_type::from_promise(*this)}; }
    void return_void() {}
    void unhandled_exception() { std::exit(1); }

    template <typename A> // no conversion, just for demo
    [[nodiscard]] auto await_transform(const A &a) {
      std::puts("Pass thru await_transform()");
      return a;
    }
  };

  handle_type m_co_handle;
};

Task generate() {
  auto r1 = co_await Expr1{};
  std::cout << "r1 = " << r1 << '\n';
  auto r2 = co_await Expr2{};
  std::cout << "r2 = " << r2 << '\n';
  auto r3 = co_await Awaitable{"from Awaitable"};
  std::cout << "r3 = " << r3 << '\n';
}

int main() {
  auto g = generate();
  g.next();
  g.next();
  g.next();
  g.next();
}

#endif