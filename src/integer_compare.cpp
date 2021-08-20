#include <version>
#ifndef __cpp_lib_integer_comparison_functions
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_integer_comparison_functions)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <iostream>
#include <utility>

int main() {
  int a = -1;
  unsigned int b = 1;

  std::cout << "Naive comparaison:\n";
  if (a < b) {
    std::cout << a << " < " << b << '\n';
  } else {
    std::cout << a << " ≥ " << b << '\n';
  }
  static_assert(static_cast<unsigned int>(-1) == 4'294'967'295);

  std::cout << "std:: comparaison:\n";
  if (std::cmp_less(a, b)) {
    std::cout << a << " < " << b << '\n';
  } else {
    std::cout << a << " ≥ " << b << '\n';
  }

  // std::cmp_less(-1.0, 1u); // error: only for integer values
}

#endif