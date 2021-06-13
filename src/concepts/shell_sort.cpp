#include <version>
#if !defined(__cpp_concepts) || !defined(__cpp_lib_concepts)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <algorithm>
#include <concepts>
#include <iostream>
#include <iterator>
#include <list>
#include <utility>
#include <vector>
//#endregion

template <std::random_access_iterator Iter, class Comp = std::less<>>
requires std::indirect_binary_predicate<Comp, Iter, Iter>
void shell_sort(Iter begin, Iter end, Comp less = {}) {
  for (auto gap = (end - begin) / 2; gap != 0; gap /= 2) {
    for (auto i = begin + gap; i != end; ++i) {
      for (auto j = i; j >= begin + gap && less(*j, j[-gap]); j -= gap) {
        std::iter_swap(j - gap, j);
      }
    }
  }
}

int main() {
  std::vector<int> v{1, 7, 2, 6, 3, 5, 4, 0};
  // std::list<int> v{1, 7, 2, 6, 3, 5, 4, 0}; // TODO: what happens with std::list?
  shell_sort(v.begin(), v.end(), std::greater<>());
  for (auto &&i : v) {
    std::cout << i << '\n';
  }
}

#endif