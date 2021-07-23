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

//#region [Includes and Debug]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <string>
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <cassert>
#include <compare>
#include <iostream>
#include <string>
#include <tuple>
//#endregion

struct A {
  int a{};
  std::string b{};
  double c{};

  std::partial_ordering operator<=>(const A &that) const {
    // This is not the default order
    if (auto cmp = b <=> that.b; cmp != 0) // Warning: deduced 'auto'
      return cmp;                          //  return type
    if (auto cmp = c <=> that.c; cmp != 0) //  could be incompatible:
      return cmp;                          // <- partial_ordering
    return a <=> that.a;                   // <- strong_ordering
  }

  // HINT The shorter way
  // auto operator<=>(const A &that) const {
  //   return std::tie(b, c, a) <=> std::tie(that.b, that.c, that.a);
  // }

  bool operator==(const A &that) const = default;

  // HINT The safest way to synchronize with operator<=>
  // bool operator==(const A &that) const { return *this <=> that == 0; }
};

int main() {
  A a{1, "a"}, b{1, "b"};
  assert(a <= b);
  auto cmp1 = a <=> a;
  std::cout << "A <=> comparaison type is: " << type<decltype(cmp1)>() << '\n';
  assert(a <=> a == std::strong_ordering::equivalent);
  auto cmp2 = a != b;
  std::cout << "A == comparaison type is: " << type<decltype(cmp2)>() << '\n';
  assert(a != b);
}

#endif