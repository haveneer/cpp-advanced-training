#include <version>
#ifndef __cpp_concepts
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Collapse all]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <iostream>
#include <list>
#include <vector>
//#endregion

template <typename T>
concept Iterable = requires(T t) {
  std::begin(t);
};

template <typename V>
requires Iterable<V>
void f(const V &v) {
  std::cout << "Iterable implementation with V=" << type<V>() << '\n';
}

template <typename V>
requires(!Iterable<V>) //
void f(const V &v) {
  std::cout << "not Iterable implementation with V=" << type<V>() << '\n';
}

// NB: MSVC 19.27-28 support negation of concept (but removed in later release)
// template <!Iterable V>
// void f(const V &v) {
//  std::cout << "not Iterable implementation with V=" << type<V>() << '\n';
//}

int main() {
  int array[10];
  std::vector<int> vector;
  std::list<int> list;

  f(1);
  f(array);
  f(vector);
  f(list);
}

#endif