#include <version>
#if !defined(__cpp_lib_constexpr_vector) || !defined(__cpp_lib_constexpr_algorithms)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES(STR(__cpp_lib_constexpr_vector),
                STR(__cpp_lib_constexpr_algorithms));
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <algorithm>
#include <cassert>
#include <iostream>
#include <vector>

constexpr int f() {
  std::vector v = {2, 4, 5, 3};
  std::vector<int> v2;
  for (auto e : v) {
    v2.push_back(e);
    v2.push_back(e / 2);
  }
  std::sort(v2.begin(), v2.end());
  return v2.front() + v2.back();
}

int main() {
  constexpr auto x = f();
  static_assert(x == 6);
}

#endif