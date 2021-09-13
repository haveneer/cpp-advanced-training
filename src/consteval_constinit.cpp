#include <version>
#if !defined(__cpp_consteval) || !defined(__cpp_constinit)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_consteval), STR(__cpp_constinit)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Declarations]
#include <iostream>
#include <tuple>
#include <type_traits>
//#endregion

// A simple consteval function
consteval std::tuple<int, bool> factorial(int n) noexcept {
  // Note: When directly used as the condition of static_assert declaration or
  // constexpr if statement, std::is_constant_evaluated() always returns true.
  return {n < 2 ? 1 : n * std::get<0>(factorial(n - 1)),
          std::is_constant_evaluated()};
}

constexpr auto constexpr_fact5 = std::get<0>(factorial(5));
constinit auto constinit_fact5 = std::get<0>(factorial(5)); // static storage

int main() {
  constexpr auto r = factorial(5); // structured binding not allowed here
  constinit thread_local auto f5 = std::get<0>(r); // thread local storage 
  constinit thread_local auto b1 = std::get<1>(r); 
  std::cout << "5! = " << f5 << " compile-time=" << b1 << '\n';

  int x = std::rand() % 20;
  // auto [frand, b2] = factorial(x); // error: the value of 'x' is not usable
  //                                  //        in a constant expression
  // std::cout << x << "! = " << frand << " compile-time=" << b2 << '\n';
}

#endif