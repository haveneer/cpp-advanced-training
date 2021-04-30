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
#include <algorithm>
#include <cassert>
#include <cctype>
#include <iostream>
#include <ranges>
#include <string>
#include <vector>
//#endregion

#include "custom_rnd_view.hpp"

int main() {
  namespace ranges = std::ranges;
  namespace views = std::views;

  unsigned int seed = 1;

  /* </dev/urandom            |
   *       tr -dc 'a-fA-F0-9' |
   *       head -c15          |
   *       fold -w 3          |
   *       paste -sd-
   */

  auto urandom = custom_views::rnd(seed) //
                 | views::transform([](int c) -> unsigned char { return c % 256; });
  auto data =                                                                 //
      urandom                                                                 //
      | views::filter([](const unsigned char c) { return std::isxdigit(c); }) //
      | views::take(15)                                                       //
                        //   | fold(3) //
      ;

  ranges::for_each(data, [](auto &&x) { std::cout << x << " "; });
  std::cout << "\n";
  
//  auto result = std::reduce(ranges::begin(data), ranges::end(data), std::string{},
//                                [](std::string acc, const char &x) {
//                                  return std::move(acc) + "_" + std::to_string(x);
//                                });
//  std::cout << "result = " << result << "\n";
}

#endif
