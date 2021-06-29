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
#include "colorize.hpp"
#include <array>
#include <cassert>
#include <iostream>
#include <list>
#include <mutex>
#include <span>
#include <sstream>
#include <thread>
#include <vector>
//#endregion

//#region [Tools]
template <typename... Args> void print(std::ostream &out, Args... args) {
  std::ostringstream oss;
  ((oss << args), ...);
  out << oss.str();
}

struct SyncStream {
  SyncStream(std::ostream &o) : o(o), oss{} {
    oss << "[" << Colorize{std::this_thread::get_id()} << "] ";
  }
  ~SyncStream() {
    oss << '\n';
    o << oss.str();
  }
  template <typename T> auto &operator<<(T &&val) { return oss << val; }
  std::ostream &o;
  std::ostringstream oss;
};

auto &operator<<(std::ostream &o, const std::span<std::byte> &data) {
  std::ostream hexout(o.rdbuf()); // safe RAII flags restore
  hexout << std::hex << "{ ";
  for (int not_first = 0; auto &&x : data) {
    if (not_first++)
      hexout << ' ';
    hexout << (int)x;
  }
  hexout << " }";
  return o;
}
//#endregion

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
    std::suspend_never initial_suspend() {
      assert(context == nullptr);
      return {};
    }
    std::suspend_always final_suspend() noexcept {
      // DO NOT USE std::suspend_never
      // it will imply an automatic destroy of the coroutine
      assert(context != nullptr);
      SyncStream{std::cout} << "Final suspend on "
                            << Colorize{handle_type::from_promise(*this).address()};
      return {};
    }
    void return_void() {
      SyncStream{std::cout} << "Return void on "
                            << Colorize{handle_type::from_promise(*this).address()};
    }
    void unhandled_exception() {}

    Context *context = nullptr;
  };

  handle_type m_co_handle;
};

size_t generate(std::byte *data, const size_t nval) {
  for (size_t i = 0; i < nval; ++i) {
    // SyncStream{std::cout} << "Add data...";
    std::this_thread::sleep_for(std::chrono::milliseconds(66));
    data[i] = std::byte(0x10 + i);
  }
  return nval;
}

struct Source {
  auto async_read_some(std::byte *data, size_t n) {
    struct Awaitable {
      bool await_ready() const { return false; }
      auto await_suspend(std::coroutine_handle<> h) {
        SyncStream{std::cout} << "Suspend for read from " << src->name
                              << " (h=" << Colorize{h.address()} << ")";
        assert(src->ncount <= n);
        --this->src->ncount;

        constexpr bool do_async = true; // HINT could be changed
        if constexpr (do_async) {
          auto job = [this, h]() mutable { // mutable required by clang-12
            this->n = generate(data, src->ncount);
            SyncStream{std::cout} << "Async Resume from " << src->name
                                  << " this->n=" << this->n
                                  << " (h=" << Colorize{h.address()} << ")";
            h.resume(); // resume() is not const in clang-12
          };
          std::thread t{job};
          t.detach();
          return true;
        } else {
          this->n = generate(data, src->ncount);
          SyncStream{std::cout} << "Sync Resume from " << src->name
                                << " this->n=" << this->n
                                << " (h=" << Colorize{h.address()} << ")";
          return false;
        }
      }
      [[nodiscard]] size_t await_resume() const {
        SyncStream{std::cout} << "Resumed with n=" << n;
        return n;
      }

      std::byte *data;
      size_t n;
      Source *src;
    };
    return Awaitable{data, n, this};
  }

  std::string name;
  size_t ncount = 3;
};

struct Destination {
  auto async_write(const std::span<std::byte> &data) {
    struct Awaitable {
      bool await_ready() const { return false; }
      auto await_suspend(std::coroutine_handle<> h) {
        SyncStream{std::cout} << "Suspend for write from " << dst->name
                              << " (h=" << Colorize{h.address()} << ")";

        constexpr bool do_async = true; // HINT could be changed
        if constexpr (do_async) {
          auto job = [this, h]() mutable { // mutable required by clang-12
            SyncStream{std::cout} << "Async Write to " << dst->name
                                  << " (size=" << data.size() << "): " << data
                                  << " (h=" << Colorize{h.address()} << ")";
            h.resume(); // resume() is not const in clang-12
          };
          std::thread t{job};
          t.detach();
          return true;
        } else {
          SyncStream{std::cout} << "sync Write to " << dst->name
                                << " (size=" << data.size() << "): " << data
                                << " (h=" << Colorize{h.address()} << ")";
          return false;
        }
      }
      void await_resume() const {
        SyncStream{std::cout} << "Resumed from " << dst->name;
      }

      std::span<std::byte> data;
      Destination *dst;
    };
    return Awaitable{data, this};
  }

  std::string name;
};

struct Context {
  auto schedule() {
    struct Awaitable {
      Awaitable(Context *context) : context(context) {}
      bool await_ready() const { return false; }
      auto await_suspend(std::coroutine_handle<> h) {
        SyncStream{std::cout} << "Save context into promise"
                              << " (h=" << Colorize{h.address()} << ")";
        auto co_handle =
            std::coroutine_handle<Task::promise_type>::from_address(h.address());
        auto &promise = co_handle.promise();
        promise.context = context;
        return false;
      }
      void await_resume() const {}

      Context *context = nullptr;
    };
    return Awaitable{this};
  }
};

Task tcp_echo_server(Source src, Destination dest, Context &ctxt) {
  co_await ctxt.schedule(); // TODO save thread context into coroutine promise

  std::byte data[64];
  for (;;) {
    SyncStream{std::cout} << src.name << "->" << dest.name << ": Step A";
    size_t n = co_await src.async_read_some(data, std::size(data));
    if (n == 0) {
      SyncStream{std::cout} << src.name << "->" << dest.name
                            << ": end signal received";
      co_return;
    }
    SyncStream{std::cout} << src.name << "->" << dest.name << ": Step B";
    co_await dest.async_write(std::span(std::begin(data), n));
    SyncStream{std::cout} << src.name << "->" << dest.name << ": Step C";
  }
  throw std::runtime_error{"Unexpected call"};
}

int main() {
  Context context;
  
  SyncStream{std::cout} << "Enter main";
  std::list<Task> tasks;
  SyncStream{std::cout} << "New Echo Stream";
  tasks.emplace_back(tcp_echo_server(Source{"Src1"}, Destination{"Dst1"}, context));
  SyncStream{std::cout} << "Check state on " << Colorize{&tasks.back()} << " "
                        << Colorize{tasks.back().m_co_handle.address()} << " "
                        << tasks.back().m_co_handle.done();
  SyncStream{std::cout} << "New Echo Stream";
  tasks.emplace_back(tcp_echo_server(Source{"Src2"}, Destination{"Dst2"}, context));
  SyncStream{std::cout} << "Check state on " << Colorize{&tasks.back()} << " "
                        << Colorize{tasks.back().m_co_handle.address()} << " "
                        << tasks.back().m_co_handle.done();
  assert((tasks.begin())->m_co_handle != (std::next(tasks.begin()))->m_co_handle);

  SyncStream{std::cout} << "Wait loop";
  while (not tasks.empty()) {
    SyncStream{std::cout} << "• (remaining=" << tasks.size() << ")";
    for (auto i = tasks.begin(); i != tasks.end(); ++i) {
      const Task &task = *i;
      if (task.done()) {
        SyncStream{std::cout} << "## Task " << Colorize{task.m_co_handle.address()}
                              << " done";
        tasks.erase(i);
      }
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
  }

  SyncStream{std::cout} << Colorize<std::thread::id>::count()
                        << " threads have been built";
  SyncStream{std::cout}
      << Colorize<decltype(std::declval<Task::handle_type>().address())>::count()
      << " coroutine handles have been built";

  SyncStream{std::cout} << "Exit main";
}

#endif