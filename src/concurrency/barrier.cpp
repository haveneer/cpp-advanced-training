#include <version>
#if !defined(__cpp_lib_barrier) || !defined(__cpp_lib_syncbuf) || \
    !defined(__cpp_lib_jthread)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_barrier), STR(__cpp_lib_syncbuf),
                 STR(__cpp_lib_jthread)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <array>
#include <barrier>
#include <iostream>
#include <mutex>
#include <syncstream>
#include <thread>
#include <vector>
//#endregion

int main() {
  std::array names = {"Krusty", "Bart", "Homer", "Ned", "Maggie"};
  bool do_run = true;
  std::barrier barrier(names.size(), [&do_run, round = 1]() mutable noexcept {
    std::cout << "\nRound #" << round << " done\n";
    if ((do_run = (++round < 3))) // compact form: don't do that
      std::cout << "Go next round\n\t";
  });
  std::cout << "Start first round\n\t";

  std::vector<std::jthread> runners;
  for (auto name : names) {
    runners.emplace_back([&, name]() {
      while (do_run) {
        std::cout << name << " ";
        barrier.arrive_and_wait();
      }
    });
  }
}

#endif