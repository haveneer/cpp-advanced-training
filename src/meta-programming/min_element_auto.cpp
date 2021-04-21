#include <version>
#ifndef my_cpp_abbreviated_function_template
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(my_cpp_abbreviated_function_template)})
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <iostream>
#include <list>
//#endregion

auto min_element(auto first, auto last) { // all auto are like template<...>
  if (first == last)                      // but they are all independent:
    return last;                          // template<typename I1, typename I2>
                                          // auto min_element(I1 first, I2 last)
  auto smallest = first;
  ++first;
  for (; first != last; ++first) {
    if (*first < *smallest) {
      smallest = first;
    }
  }
  return smallest;
}

int main() {
  {
    int v[] = {5, 4, 1, 8, 4};
    auto minp = ::min_element(std::begin(v), std::end(v));
    std::cout << "min value is " << *minp << '\n';
  }

  {
    std::list<double> v = {{5.1, 4.1, 1.1, 8.1, 4.1}};
    auto minp = ::min_element(std::begin(v), std::end(v));
    std::cout << "min value is " << *minp << '\n';
  }
}

#endif