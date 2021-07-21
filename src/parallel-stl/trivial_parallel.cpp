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
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
//#endregion

int main() {
  std::vector<double> data(1'000'000);
  std::mt19937 engine{2021};
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  std::generate(std::begin(data), std::end(data), [&] { return dist(engine); });

  const auto start1 = std::chrono::steady_clock::now();
  
  std::sort(data.begin(), data.end()); // HINT compare this ...
  
  const auto end1 = std::chrono::steady_clock::now();
  std::cout << std::chrono::duration<double, std::milli>(end1 - start1).count()
            << " ms\n";

  const auto start2 = std::chrono::steady_clock::now();
  
  std::sort(std::execution::par, data.begin(), data.end()); // HINT ... to this
  
  const auto end2 = std::chrono::steady_clock::now();
  std::cout << std::chrono::duration<double, std::milli>(end2 - start2).count()
            << " ms\n";
}

#endif