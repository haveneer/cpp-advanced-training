#include <version>
#if !defined(__cpp_lib_constexpr_algorithms)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_constexpr_algorithms)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Includes]
#include <algorithm>
#include <array>
//#endregion

constexpr int f() {
  std::array v = {2, 4, 1, 5, 3}; // vector in constexpr context
  std::sort(v.begin(), v.end());  // call STL non-trivial algorithm
  return v.front() + v.back();
}

int main() {
  constexpr auto x = f();
  static_assert(x == 6); // result computed at compile time
}

#endif