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
//#endregion

//#region [Debug stuff]
#include <cassert>
#include <utility>
#include <vector>
#include <cstdlib>
//#endregion

struct Task {
  struct promise_type {
    Task get_return_object() {
      return std::coroutine_handle<promise_type>::from_promise(*this);
    }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() { std::exit(1); }
  };

  using handle_type = std::coroutine_handle<promise_type>;
  Task(handle_type m) : m_co_handle(m) {}
  ~Task() {
    if (m_co_handle)
      m_co_handle.destroy();
  }

#if 0 // TODO What happens if copy management is not done 
  Task(Task &) = delete;
  Task &operator=(Task &) = delete;
  Task(Task &&that) : m_co_handle(std::exchange(that.m_co_handle, nullptr)) {}
  Task &operator=(Task &&that) {
    if (m_co_handle) m_co_handle.destroy();
    m_co_handle = std::exchange(that.m_co_handle, nullptr);
    return *this;
  }
#endif

  handle_type m_co_handle;
};

Task new_task() { co_await std::suspend_always{}; }

int main() {
  Task c = new_task();
  Task c2 = new_task();
  assert(c.m_co_handle != c2.m_co_handle);

  std::vector<Task> tasks;
  tasks.emplace_back(new_task());
  tasks.emplace_back(new_task());
  assert(tasks[0].m_co_handle != tasks[1].m_co_handle); // Two coroutine handles
                                                  // could not be equal ! (cf TODO)
}

#endif