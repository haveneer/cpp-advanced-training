#include <version>
#ifndef __cpp_lib_ranges
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_ranges)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <algorithm>
#include <iostream>
#include <vector>
#include <ranges>

int main() {
  std::vector<int> container = {51, 25,   54, 78, 52, 1,   45, 78,
                                42, 1245, 1,  48, 1,  245, 4,  5};
  std::ranges::sort(container);

  for (auto &&x : container) {
    std::cout << x << ' ';
  }
}

#endif