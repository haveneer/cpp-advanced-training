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

//#region [Verify GCC fix]
#ifdef __GNUC__
#ifndef __clang__
#if __GNUC__ >= 12
#define SHOULD_BE_FIXED_IN_GCC12
#endif
#endif
#endif
//#endregion

#include <ranges>
#include <cassert>

int main() {
  auto v =
      std::views::single(1)                                                        //
      | std::views::transform([](const auto &s) { return std::views::single(s); }) //
      | std::views::join;

#if defined(_MSC_VER) || defined(SHOULD_BE_FIXED_IN_GCC12) 
  assert(std::ranges::next(v.begin()) == v.end());
#else
  // When this assertion will fail, the bug will be solved
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=100290
  assert(std::ranges::next(v.begin()) != v.end());
#endif
}

#endif