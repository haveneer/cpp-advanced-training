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
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <iostream>
#include <list>
#include <vector>
//#endregion
//#region [Definitions]

template <typename T>
concept Indexable = requires(T t) {
  t[1];
};

template <typename V>
requires Indexable<V>
void f(const V &v) {
  std::cout << "Iterable implementation with V=" << type<V>() << '\n';
}

constexpr bool negate(bool b) { return !b; }

template <typename V>
requires requires { negate(Indexable<V>); }
void f(const V &v) {
  std::cout << "not Indexable implementation with V=" << type<V>() << '\n';
}

template <typename T>
concept int_sized = requires {
  sizeof(T) == sizeof(int);
};
static_assert(int_sized<int[128]>); // FIXME

int main() {
  std::vector<int> vector;
  std::list<int> list;
  
  f(vector); // error: call of overloaded is ambiguous; FIXME
  //#region [answer]
  // 'requires { negate(Indexable<V>); }' is not the negation of Indexable concept;
  // it only checks that you can apply negate on Indexable
  // (i.e. negation of a bool)
  //#endregion
  f(list); // not Indexable
}

#endif