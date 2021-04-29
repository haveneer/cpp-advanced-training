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

// https://www.fluentcpp.com/2019/02/15/how-smart-output-iterators-fare-with-the-terrible-problem-of-incrementing-a-smart-iterator/

//#region [Headers]
#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>
//#endregion

#include <cassert>
#include <iostream>
#include <ranges>

int main() {
  int n = 0;
  auto gen = [&n]() { // it could also be a file data reader
    auto result = ++n;
    std::cout << "Generate [" << result << "]\n";
    return result;
  };

  auto tmp =                                                     //
      std::views::iota(0)                                        //
      | std::views::transform([gen](auto &&c) { return gen(); }) //
      | std::views::filter([](auto &&i) {
          std::cout << "#1 " << i << " " << (i % 2) << "\n";
          return (i % 2) == 1;
        });

  for (auto &&i : tmp | std::views::take(1)) {
    std::cout << "#2 " << i << " " << ((i % 2) == 1) << "\n";
    // The generator has been called after filtering
    // The value is no more well filtered
    // assert(((i % 2) == 1)); // this assertion will fail
  }
}

#endif