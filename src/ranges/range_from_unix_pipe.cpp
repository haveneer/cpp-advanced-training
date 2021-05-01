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
#include "custom_window_fold_view.hpp"

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
      | custom_views::window_fold<std::string>(3)                             //
      ;

  std::vector<std::string> vdata;
  ranges::copy(data, std::back_inserter(vdata));
  // accumulate cannot operate directly on ranges
  auto result = std::accumulate(vdata.begin(), vdata.end(), std::string{},
                                [](std::string s, auto &&x) {
                                  if (s.empty()) {
                                    return x;
                                  } else {
                                    return std::move(s) + '-' + x;
                                  }
                                });
  std::cout << "result = " << result << "\n";
}

#endif
