#include <version>
#ifndef __cpp_lib_parallel_algorithm
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_parallel_algorithm)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <algorithm>
#include <execution>
#include <iostream>
#include <stdexcept>
#include <vector>

int main() {
  std::vector<int> v{1, 2, 3, 4, 5};

  try {
    std::for_each(v.begin(), v.end(), [](auto) {
      throw std::runtime_error("Without execution policy");
    });
  } catch (const std::runtime_error &e) { // exception caught here
    std::cout << e.what() << std::endl;
  }

  try {
    std::for_each(std::execution::seq, v.begin(), v.end(),
                  [](auto) { throw std::runtime_error("With execution policy"); });
  } catch (const std::runtime_error &e) { // exception never caught
    std::cout << e.what() << std::endl;
  } catch (...) {
    std::cout << "Catch-all exceptions" << std::endl;
  }
}

#endif