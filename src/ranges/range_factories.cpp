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

#include <iostream>
#include <ranges>
#include <sstream>

int main() {
  using namespace std::ranges;
  namespace ranges = std::ranges;
  // std::views is a namespace alias of std::ranges::views

  for (auto &&x : views::empty<int>) {
    std::cout << x << ' ';
  }
  std::cout << '\n';

  for (auto &&x : views::single(1)) {
    std::cout << x << ' ';
  }
  std::cout << '\n';

  for (auto &&x : views::iota(1, 10)) {
    std::cout << x << ' ';
  }
  std::cout << '\n';

  std::istringstream iss("Buvez de ce whisky que le patron juge fameux");
  for (auto &&x : ranges::istream_view<std::string>(iss)) {
    std::cout << x << ' ';
  }
  std::cout << '\n';
}

#endif