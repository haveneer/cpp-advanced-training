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
//#endregion

//#region [Debug stuff]
#include <iostream>
#include <string>
#include <vector>
#ifndef _MSC_VER
#define PRINTME() std::cout << __PRETTY_FUNCTION__ << '\n'
#else
#define PRINTME() std::cout << __FUNCSIG__ << '\n'
#endif
//#endregion

struct HelloWorldCoro {
  // clang-format off
  struct promise_type; // type required by compiler; defined later

  HelloWorldCoro(promise_type *p)
      : m_handle(std::coroutine_handle<promise_type>::from_promise(*p)) { PRINTME(); }
  ~HelloWorldCoro() { PRINTME(); /* m_handle.destroy(); */ }

  struct promise_type {
    promise_type() { PRINTME(); }
    ~promise_type() { PRINTME(); }
    HelloWorldCoro get_return_object() { PRINTME(); return {this}; }
    std::suspend_always initial_suspend() { PRINTME(); return {}; }
    std::suspend_never final_suspend() noexcept { PRINTME(); return {}; }
    void return_void() { PRINTME(); }
    void unhandled_exception() { PRINTME(); std::exit(1); }
  };

  std::coroutine_handle<promise_type> m_handle;
  // clang-format on  
};

HelloWorldCoro helloWorld() {
  // should suspend here ? (cf promise_type::initial_suspend) (A)
  PRINTME();
  // first build coroutine object (promise_type::using get_return_object)
  std::cout << "Hello \n";
  co_await std::suspend_always{}; // (B)
  std::cout << "World!" << std::endl;
  PRINTME();
  // should suspend here ? (cf promise_type::final_suspend)
} // destroy promise_type - by ~HelloWorldCoro if final_suspend is suspend_always
  //                      - itself if final_suspend is suspend_never

int main() {
  HelloWorldCoro c = helloWorld(); // new coroutine

  PRINTME();
  c.m_handle.resume(); // (A) : usually, m_handle is wrapped in Coro object 
  PRINTME();          
  c.m_handle();        // (B) : equivalent to c.m_handle.resume();
  PRINTME();
}

#endif