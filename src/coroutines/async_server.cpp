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
#include <array>
#include <iostream>
#include <span>
//#endregion

struct Task {
  struct promise_type {
    Task get_return_object() { return {}; }
    std::suspend_never initial_suspend() { return {}; }
    std::suspend_never final_suspend() noexcept { return {}; }
    void return_void() {}
    void unhandled_exception() {}
  };
};

struct Source {
  std::suspend_always async_read_some(std::byte *data, size_t n) { return {}; }
};

struct Destination {
  std::suspend_always async_write(const std::span<std::byte> &data) { return {}; }
};

Task tcp_echo_server(Source src, Destination dest);

int main() {
  
  
  
  tcp_echo_server(Source{}, Destination{}); 


}

// Async data using istream ?
// using mkfifo 

Task tcp_echo_server(Source src, Destination dest) {
  std::byte data[1024];
  for (;;) {
    size_t n = 0;
    co_await src.async_read_some(data, std::size(data));
    co_await dest.async_write(std::span(std::begin(data), n));
  }
}

// task<int> async_sum(channel& s) { int sum = 0;
//   for (;;)
//     sum += co_await s.async_read<int>(); co_return sum;
// }

#endif