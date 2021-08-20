#include <version>
#if !defined(__cpp_impl_three_way_comparison) || \
    !defined(__cpp_lib_three_way_comparison)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_impl_three_way_comparison),
                 STR(__cpp_lib_three_way_comparison)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Headers]
#include <iostream>
//#endregion

//#region [Custom 2D array]
struct Index {
  int value;
  void operator++() { ++value; }
  auto operator<=>(const Index &) const = default;
  auto operator<=>(const int &v) const { return this->value <=> v; };
};

struct Index2 {
  int i, j;
};

Index2 operator,(Index i, Index j) { return {i.value, j.value}; };

struct Array {
  int data[9];
  int operator[](Index2 ij) { return data[3 * ij.i + ij.j]; }
};
//#endregion

int main() {
  Array array = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  for (Index i{}; i < 3; ++i) {
    for (Index j{}; j < 3; ++j) {
      auto v = array[i, j]; // warning: top-level comma expression in
                            // array subscript is deprecated
      v = array[(i, j)];    // OK, without warning
      std::cout << v << " ";
    }
    std::cout << '\n';
  }
}

#endif