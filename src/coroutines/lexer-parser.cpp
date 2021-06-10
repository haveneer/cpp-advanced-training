#include <version>
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

#include <coroutine>
#include <iostream>
#include <stdexcept>

struct HelloWorldCoro {
  struct promise_type { // compiler looks for `promise_type`
    void unhandled_exception() { std::exit(1); }
    HelloWorldCoro get_return_object() { return this; }
    std::suspend_always initial_suspend() { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }
  };

  HelloWorldCoro(promise_type *p)
      : m_handle(std::coroutine_handle<promise_type>::from_promise(*p)) {}
  ~HelloWorldCoro() { m_handle.destroy(); }

  std::coroutine_handle<promise_type> m_handle;
};

HelloWorldCoro print_hello_world() {
  std::cout << "Hello ";
  co_await std::suspend_always{};
  std::cout << "World!" << std::endl;
}

int main() {
  // https://stackoverflow.com/questions/57962476/gcc-clang-lexer-and-parser
  
  HelloWorldCoro mycoro = print_hello_world();

  mycoro.m_handle.resume();
  mycoro.m_handle(); // Equal to mycoro.m_handle.resume();
  return EXIT_SUCCESS;
}

#endif