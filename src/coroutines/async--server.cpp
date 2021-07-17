#include <version>
#if __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif

#ifdef __cpp_lib_coroutine

#include "async--server.hpp"
#include "async--synccout.hpp"

#include <cassert>
#include <thread>

/// Tools

template <typename... Args> void print(std::ostream &out, Args... args) {
  std::ostringstream oss;
  ((oss << args), ...);
  out << oss.str();
}

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

/// Task

std::suspend_never Task::promise_type::initial_suspend() {
  assert(context == nullptr);
  return {};
}

std::suspend_always Task::promise_type::final_suspend() noexcept {
  // DO NOT USE std::suspend_never
  // it will imply an automatic destroy of the coroutine
  assert(context != nullptr);
  SyncCout{} << "Final suspend on "
             << Colorize{handle_type::from_promise(*this).address()};
  return {};
}

void Task::promise_type::return_void() {
  SyncCout{} << "Return void on "
             << Colorize{handle_type::from_promise(*this).address()};
}

/// Context

bool Context::Awaitable::await_suspend(std::coroutine_handle<> h) {
  SyncCout{} << "Save context into promise"
             << " (h=" << Colorize{h.address()} << ")";
  auto co_handle =
      std::coroutine_handle<Task::promise_type>::from_address(h.address());
  auto &promise = co_handle.promise();
  promise.context = context;
  return false;
}

// Data generator

size_t generate(std::byte *data, const size_t nval) {
  for (size_t i = 0; i < nval; ++i) {
    // SyncCout{} << "Add data...";
    std::this_thread::sleep_for(std::chrono::milliseconds(66));
    data[i] = std::byte(0x10 + i);
  }
  return nval;
}

/// Source

bool Source::Awaitable::await_suspend(std::coroutine_handle<> h) {
  SyncCout{} << "Suspend for read from " << src->name
             << " (h=" << Colorize{h.address()} << ")";
  assert(src->ncount <= n);
  --this->src->ncount;

  constexpr bool do_async = true; // HINT could be changed
  if constexpr (do_async) {
    auto job = [this, h]() mutable { // mutable required by clang-12
      this->n = generate(data, src->ncount);
      SyncCout{} << "Async Resume from " << src->name << " this->n=" << this->n
                 << " (h=" << Colorize{h.address()} << ")";
      h.resume(); // resume() is not const in clang-12
    };
    std::thread t{job};
    t.detach();
    return true;
  } else {
    this->n = generate(data, src->ncount);
    SyncCout{} << "Sync Resume from " << src->name << " this->n=" << this->n
               << " (h=" << Colorize{h.address()} << ")";
    return false;
  }
}

size_t Source::Awaitable::await_resume() const {
  SyncCout{} << "Resumed with n=" << n;
  return n;
}

/// Destination

bool Destination::Awaitable::await_suspend(std::coroutine_handle<> h) {
  SyncCout{} << "Suspend for write from " << dst->name
             << " (h=" << Colorize{h.address()} << ")";

  constexpr bool do_async = true; // HINT could be changed
  if constexpr (do_async) {
    auto job = [this, h]() mutable { // mutable required by clang-12
      SyncCout{} << "Async Write to " << dst->name << " (size=" << data.size()
                 << "): " << data << " (h=" << Colorize{h.address()} << ")";
      h.resume(); // resume() is not const in clang-12
    };
    std::thread t{job};
    t.detach();
    return true;
  } else {
    SyncCout{} << "sync Write to " << dst->name << " (size=" << data.size()
               << "): " << data << " (h=" << Colorize{h.address()} << ")";
    return false;
  }
}

void Destination::Awaitable::await_resume() const {
  SyncCout{} << "Resumed from " << dst->name;
}

/// Server

Task tcp_echo_server(Source src, Destination dest, Context &ctxt) {
  co_await ctxt.schedule(); // TODO save thread context into coroutine promise

  std::byte data[64];
  for (;;) {
    SyncCout{} << src.name << "->" << dest.name << ": Step A";
    size_t n = co_await src.async_read_some(data, std::size(data));
    if (n == 0) {
      SyncCout{} << src.name << "->" << dest.name << ": end signal received";
      co_return;
    }
    SyncCout{} << src.name << "->" << dest.name << ": Step B";
    co_await dest.async_write(std::span(std::begin(data), n));
    SyncCout{} << src.name << "->" << dest.name << ": Step C";
  }
  throw std::runtime_error{"Unexpected call"};
}

#endif