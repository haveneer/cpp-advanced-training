#include <version>
#if !defined(__cpp_lib_latch) || !defined(__cpp_lib_syncbuf) || \
    !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_latch), STR(__cpp_lib_syncbuf),
                 STR(__cpp_lib_jthread)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <array>
#include <iostream>
#include <latch>
#include <mutex>
#include <syncstream>
#include <thread>
#include <vector>
//#endregion

int main() {
  std::array names = {"Krusty", "Bart", "Homer", "Ned", "Maggie"};
  std::latch latch(names.size());
  std::cout << "Come on guys !\n\t";

  std::vector<std::jthread> runners;
  for (auto name : names) {
    runners.emplace_back([&, name]() {
      std::osyncstream(std::cout) << name << " ";
      latch.count_down(); // don't wait
    });
  }

  latch.wait();
  std::cout << "\nEverybody is there" << std::endl;
}

#endif