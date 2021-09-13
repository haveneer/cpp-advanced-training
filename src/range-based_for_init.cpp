#include <version>
#if !defined(my_cpp_feature_range_based_for_loop_with_structured_binding_init)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES(
    {STR(my_cpp_feature_range_based_for_loop_with_structured_binding_init)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#include <iostream>
#include <string_view>
#include <vector>

struct S {
  std::string greeting = "Hello";
  std::vector<std::string_view> names = {"Thomas", "Stéphane",  "Raphaël",
                                         "Ani",    "Jean-Marc", "Sylvain",
                                         "Xavier", "Julien",    "Guillaume"};
};
//#endregion

int main() {
  std::vector v = {3, 1, 4, 1, 5, 9};
  std::cout << "v is {";
  // Common usage
  for (auto index = 0; auto x : v) {
    std::cout << ((index++ != 0) ? ", " : "") << x;
  }
  std::cout << "}\n\n";

  // More advanced // MSVC 19.29 has a bug here (cf CMakeLists.txt)
  for (const auto &[word, names] = S{}; auto name : names) {
    std::cout << word << ' ' << name << '\n';
  }
}

#endif