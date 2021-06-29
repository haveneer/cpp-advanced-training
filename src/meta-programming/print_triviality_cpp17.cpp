//#region [Collapse all]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <iostream>
#include <type_traits>
#include <vector>
//#endregion

template <typename T> //
void printTriviality(const T &t) {
  if constexpr (std::is_trivial_v<T>) { // Trivial types
    std::cout << "Trivial case: " << type<T>() << '\n';
  } else { // Non trivial types
    std::cout << "Non Trivial case: " << type<T>() << '\n';
  }
}

int main() {
  printTriviality(1);
  printTriviality(std::vector<int>{});
}