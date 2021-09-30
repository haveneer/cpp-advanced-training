#include <version>
#if !defined(__cpp_lib_bind_front)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_bind_front)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <functional>
#include <iostream>
#include <random>
int main() {
  std::random_device seed;
  auto gen = std::bind_front(std::uniform_real_distribution<>(0, 1),
                             std::default_random_engine(seed()));
  double random_data = gen();
}

#endif