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

//#region [Collapse all]
#include <iostream>
#include <ranges>
#include <vector>
//#endregion
int square(int i) { return i * i; };

int main() {
  std::vector<int> ints{0, 1, 2, 3, 4, 5};
  auto even = [](int i) { return 0 == i % 2; };

  for (int i : ints | std::views::filter(even) | std::views::transform(square)) {
    std::cout << i << ' ';
  }
}

#endif