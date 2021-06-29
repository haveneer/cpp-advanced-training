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
concept Indexable = requires(T t) {
  t[1];
};

template <typename T>
concept HasValueType = requires {
  typename T::value_type;
};

template <typename T>
concept Iterable = requires(T t) {
  std::begin(t);
};

template <typename V> // #1: no constraint : default
void f(const V &v) {
  std::cout << "Default implementation with V=" << type<V>() << '\n';
}

template <Iterable V> // #2: one constraint : ok if concept compliance
void f(const V &v) {  // else use default
  std::cout << "Iterable implementation with V=" << type<V>() << '\n';
}

template <Iterable V> // #3: one constraint + one
requires Indexable<V> // (more constrained)
void f(const V &v) {
  std::cout << "Iterable+Indexable implementation with V=" << type<V>() << '\n';
}

// template <Iterable V>    // #4: one constraint + another one
// requires HasValueType<V> // diverging => ambiguous
// void f(const V &v) {
//   std::cout << "Iterable+HasValueType implementation with V=" << type<V>() <<
//   '\n';
// }

template <Indexable V> // #5: one constraint; less constrained than #3
void f(const V &v) {   // choose #5 if Indexable but not Iterable
  std::cout << "Indexable implementation with V=" << type<V>() << '\n';
}

// template <HasValueType V> // #6: one distinct constraint
// void f(const V &v) {      // no case disjunction => ambigous
//   std::cout << "Indexable implementation with V=" << type<V>() << '\n';
// }

int main() {
  int array[10];
  std::vector<int> vector;
  std::list<int> list;

  f(1);
  f(array);
  f(vector); // #4, #6: error: call of overloaded 'f(std::vector<int>&)' is ambiguous
  f(list);   // #6: error: call of overloaded 'f(std::vector<int>&)' is ambiguous
}

#endif