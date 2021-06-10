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

#include <iostream>
#include <list>
#include <vector>

template <typename Container> auto iterate(const Container &c) {
  return *c.begin(); // FIXME dummy implementation
}

int main() {
  std::list<std::vector<int>> s{{1, 2, 3, 4}, {5, 6, 7, 8}};

  for (auto i : iterate(s)) {
    std::cout << i << ' ';
  }
  std::cout << "\n";
}

#endif