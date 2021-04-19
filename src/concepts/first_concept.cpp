#include <version>
#ifndef __cpp_concepts
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <type_traits>

template <typename T>
concept AlwaysOk = true;

template <typename T>
concept WhatIsWant = std::is_arithmetic_v<T> && !std::is_floating_point_v<T>;

template <WhatIsWant T> void f(T t) {} // HINT concept in action !

int main() {
  f(1);
  // f(1.0); // error: use of function 'void f(T) [with T = double]'
  //         // with unsatisfied constraints
}

#endif