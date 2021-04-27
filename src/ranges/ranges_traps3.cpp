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

//#region [Headers]
#include <algorithm>
#include <iostream>
#include <map>
#include <ranges>
#include <string>
#include <type_traits>
#include <vector>
//#endregion

using Unit = std::pair<uint64_t, char>;
static const std::vector<Unit> units = {{365, 'Y'}, {24, 'h'}, {60, 'm'}, {60, 's'}};

auto process(uint64_t &seconds, const Unit unit) -> std::string {
  uint64_t rem = seconds % unit.first;
  seconds /= unit.first;
  auto r = std::to_string(rem) + '_' + unit.second;
  std::cout << "# " << r << '\n';
  return r;
}

auto f_rev1(uint64_t seconds) {  // .rev() before process
  auto v = units                 //
           | std::views::reverse // HINT: before
           | std::views::transform([&seconds](const Unit &unit) mutable {
               return process(seconds, unit);
             });
  std::vector<std::string> s;
  std::ranges::copy(v, std::back_inserter(s));
  std::ranges::copy(s, std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << '\n';
}

auto f_rev2(uint64_t seconds) { // .rev() after process
  auto v = units                //
           | std::views::transform([&seconds](const Unit &unit) mutable {
               return process(seconds, unit);
             })                   //
           | std::views::reverse; // HINT: after
  std::vector<std::string> s;
  std::ranges::copy(v, std::back_inserter(s));
  std::ranges::copy(s, std::ostream_iterator<std::string>(std::cout, " "));
  std::cout << '\n';
}

int main() {
  uint64_t seconds = 1'000'000;
  f_rev1(seconds);
  f_rev2(seconds);
}

#endif