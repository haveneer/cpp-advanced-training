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

//#region [Includes]
#include <algorithm>
#include <execution>
#include <iostream>
#include <vector>
//#endregion

int main() {
  std::vector x{1., 2., 3., 4.};
  std::vector y{1., 2., 3., 4.};
  auto execution_policy = std::execution::par_unseq;

  auto multiply = [](double x, double y) { return x * y; };

  // Binary transform (x[i], y[i]) => x[i] = x[i] * y[i]
  std::transform(execution_policy,   // Your execution policy
                 x.begin(), x.end(), // "Left" input sequence.
                 y.begin(),          // "Right" input sequence.
                 x.begin(),          // Output sequence.
                 multiply);

  std::copy(x.begin(), x.end(), // Display result in x
            std::ostream_iterator<double>(std::cout, " "));
}

#endif