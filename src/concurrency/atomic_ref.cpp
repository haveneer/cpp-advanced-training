#include <version>
#if !defined(__cpp_lib_atomic_ref)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_atomic_ref)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <atomic>
#include <future>
#include <vector>
#include <iostream>
//#endregion

struct BigStructure {
  std::vector<int> data = {1, 2, 3, 4, 5, 5, 6};
  size_t counter = 0; // not atomic
};

void count_parity(BigStructure &s, bool count_even) {
  std::atomic_ref counter{s.counter}; // TODO what happens with atomic not _ref ?
  for (const auto &x : s.data) {
    if (x % 2 == count_even)
      ++counter;
  }
}

int main() {
  BigStructure s;

  auto fut1 = std::async(std::launch::async, count_parity, std::ref(s), true);
  auto fut2 = std::async(std::launch::async, count_parity, std::ref(s), false);

  fut1.wait();
  fut2.wait();

  if (s.counter != s.data.size())
    throw std::runtime_error("Invalid computed size");
  else
    std::cout << s.counter << " == " << s.data.size() << "\n";
}

#endif