#include <version>
#if !defined(__cpp_constinit)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_constinit)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <iostream>
//#endregion

constexpr int constexpr_value = 41; // compile time initialized IMMUTABLE data
constinit int constinit_value = 41; // compile time initialized MUTABLE data

int main() {
  auto value = 41;
  std::cout << "value incr:" << ++value << '\n';

  const auto const_value = 41; // runtime constant

  // std::cout << "const_value incr: " << ++const_value << '\n';
  //           // error: increment of read-only variable 'res'

  // std::cout << "constexpr_value incr: " << ++constexpr_value << '\n';
  // error: increment of read-only variable 'constexpr_value'

  std::cout << "constinit_value incr: " << ++constinit_value << '\n'; // OK !!

  constexpr auto local_constexpr = 41;
  // constinit auto local_constinit = 41;
  // error: 'constinit' can only be applied to a variable with static
  //        or thread storage duration
}

#endif