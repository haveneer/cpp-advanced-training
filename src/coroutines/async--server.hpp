#pragma once

#include <version>
#if __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif

#ifdef __cpp_lib_coroutine

#if __has_include(<coroutine>)
#include <coroutine>
#elif __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#endif

#include <span>
#include <string>

#include "async--thread_pool.hpp"

struct Context;

struct Task {
  struct promise_type;
  using handle_type = std::coroutine_handle<promise_type>;

  Task(handle_type h) : m_co_handle(h) {}
  ~Task() {
    if (m_co_handle)
      m_co_handle.destroy();
  }
  Task(Task &) = delete;
  Task &operator=(Task &) = delete;
  Task(Task &&that) noexcept
      : m_co_handle(std::exchange(that.m_co_handle, nullptr)) {}
  Task &operator=(Task &&that) = delete;

  void resume() { m_co_handle.resume(); }
  bool done() const { return m_co_handle.done(); }

  struct promise_type {
    ~promise_type() {}
    auto get_return_object() { return Task{handle_type::from_promise(*this)}; }
    std::suspend_never initial_suspend();
    std::suspend_always final_suspend() noexcept;
    void return_void();
    void unhandled_exception() {}

    Context *context = nullptr;
  };

  handle_type m_co_handle;
};

// FIXME if Context does not embed more data/method,
// ThreadPool should be located in place of context pointer members
struct Context {
  struct Awaitable {
    Awaitable(Context *context) : context(context) {}
    bool await_ready() const { return false; }
    bool await_suspend(std::coroutine_handle<> h);
    void await_resume() const {}
    Context *context = nullptr;
  };

  Awaitable schedule() { return Awaitable{this}; }

  ThreadPool &pool;
};

/////////////////////////

struct Source {
  struct Awaitable {
    bool await_ready() const { return false; }
    bool await_suspend(std::coroutine_handle<> h);
    [[nodiscard]] size_t await_resume() const;
    std::byte *data;
    size_t n;
    Source *src;
  };
  auto async_read_some(std::byte *data, size_t n) {
    return Awaitable{data, n, this};
  }

  std::string name;
  size_t ncount = 3;
};

struct Destination {
  struct Awaitable {
    bool await_ready() const { return false; }
    bool await_suspend(std::coroutine_handle<> h);
    void await_resume() const;
    std::span<std::byte> data;
    Destination *dst;
  };

  auto async_write(const std::span<std::byte> &data) {
    return Awaitable{data, this};
  }

  std::string name;
};

Task echo_server(Source src, Destination dest, Context &ctx);

#endif