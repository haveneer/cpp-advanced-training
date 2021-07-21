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
#include <chrono>
#include <execution>
#include <iostream>
#include <random>
#include <vector>

struct no_policy_t {
} no_policy;

template <typename T>
void test(const T &policy, const std::vector<double> &data, const int repeat,
          const char *message) {
  for (int i = 0; i < repeat; ++i) {
    std::vector<double> curr_data(data);

    const auto start = std::chrono::steady_clock::now();
    if constexpr (std::is_same_v<T, no_policy_t>)
      std::sort(curr_data.begin(), curr_data.end());
    else
      std::sort(policy, curr_data.begin(), curr_data.end());
    const auto end = std::chrono::steady_clock::now();
    auto diff = end - start;
    std::cout << message << ' '
              << std::chrono::duration<double, std::milli>(diff).count() << " ms\n";
  }
  std::cout << '\n';
}

int main() {
  // Test samples and repeat factor
  constexpr size_t samples{10'000'000};
  constexpr int repeat{3};

  // Fill a vector with samples numbers
  std::mt19937_64 engine(2021);
  std::uniform_real_distribution<double> dist(0.0, 1.0);
  
  std::vector<double> data(samples);
  std::generate(std::begin(data), std::end(data), [&] { return dist(engine); });

  // Sort data using different execution policies
  std::cout << "no execution policy\n";
  test(no_policy, data, repeat, "Elapsed time");

  std::cout << "std::execution::seq\n";
  test(std::execution::seq, data, repeat, "Elapsed time");

  std::cout << "std::execution::par\n";
  test(std::execution::par, data, repeat, "Elapsed time");

  std::cout << "std::execution::par_unseq\n";
  test(std::execution::par_unseq, data, repeat, "Elapsed time");
}

#endif