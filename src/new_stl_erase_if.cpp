#include <version>
#ifndef __cpp_lib_erase_if
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_erase_if)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Headers]
#include <algorithm>
#include <iostream>
#include <list>
#include <vector>

template <typename Container>
void print_container(const std::string_view &text, const Container &c) {
  std::cout << text << "\n";
  std::copy(c.begin(), c.end(), std::ostream_iterator<int>(std::cout, " "));
  std::cout << '\n';
}
//#endregion

int main() {
  std::vector<int> vorg{-1, 2, 3, -2, 5, 0, -1};
  print_container("Original vector", vorg);

  std::vector v = vorg;
  auto end_of_good_values =              // This is the
      std::remove_if(v.begin(), v.end(), //  remove-erase idiom
                     [](auto e) { return e < 0; });
  v.erase(end_of_good_values, v.end());
  print_container("Using remove_if idiom", v);

  v = vorg;
  std::erase_if(v, [](auto e) { return e < 0; });
  print_container("Using new std::erase_if", v);

  std::list<int> l;
  std::copy(v.begin(), v.end(), std::back_inserter(l));
  std::erase_if(l, [](auto e) { return e < 0; });
  print_container("Using same new std::erase_if on list", l);
}

#endif