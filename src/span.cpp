#include <version>
#ifndef __cpp_lib_span
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_span)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Headers]
#include <array>
#include <cassert>
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>
//#endregion
#include <span>

template <typename T> // default is dynamic extent only
void f(std::string &&name, std::span<T> container) {
  std::cout << "f(" << name << ")(size=" << std::setw(7)
            << ((container.extent == std::dynamic_extent)
                    ? "dynamic"
                    : std::to_string(container.size()))
            << ") = {";
  for (int pos = 0; auto &x : container) {
    std::cout << ((pos++ == 0) ? "" : ",") << ' ' << x;
    x++; // we can change change
  }
  std::cout << " }\n";
}

template <typename T, size_t Extent> // explicit extent; should be static or dynamic
void g(std::string &&name, std::span<T, Extent> container) {
  std::cout << "g(" << name << ")(size=" << std::setw(7)
            << ((container.extent == std::dynamic_extent)
                    ? "dynamic"
                    : std::to_string(container.size()))
            << ") = {";
  for (int pos = 0; auto x : container) {
    std::cout << ((pos++ == 0) ? "" : ",") << ' ' << x;
    x++; // we can change the value
  }
  std::cout << " }\n";
}

int main() {
  std::vector v1 = {1, 2, 3, 4};
  std::span sp1{v1};           // dynamic extent
  std::span<int, 4> sp11{sp1}; // explicit conversion dynamic to static extent
  // v1.emplace_back(5); // DON'T DO THAT; reallocation may brake span pointers
  f("v1", sp1);
  g("v1", sp1);
  // f("v1", v1);         // error: implicit conversion
  f("v1", std::span{v1}); // explicit conversion OK

  std::array v2{1., 2., 3., 4.};
  std::span sp2{v2};              // static extent (cf CTAD)
  static_assert(sp2.size() == 4); // static assertion
  std::span<double> sp22 = sp2;   // explicit conversion static to dynamic extent
  assert(sp22.size() == 4);       // dynamic assertion
  // f("v2", sp2); // error: requires a dynamic extent span
  f("v2", sp22);
  g("v2", sp2);
  //#region [More]
  std::string v3{"1234"};
  std::span sp3{v3};
  f("v3", sp3);
  g("v3", sp3);

  char v4[]{'1', '2', '3', '4'};
  // f(("v4", std::span{v4}); // error: requires a dynamic extent span
  g("v4", std::span{v4});

  std::array v5 = {0, 1, 2, 3, 4, 5};
  std::span sp5 = std::span{v5}.subspan(1, 4); // subspan
  f("v5", sp5);
  g("v5", sp5);
  for (auto &&b : std::as_bytes(sp5)) { // span as bytes (or std::as_writable_bytes)
    std::cout << std::hex << static_cast<int>(b);
  }
  //#endregion
}

#endif