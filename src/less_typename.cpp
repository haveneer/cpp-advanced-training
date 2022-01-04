//#region [Declarations]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <vector>
#ifdef my_cpp_feature_less_typename
// C++20 compilers can infer non-ambiguous typename keyword
// Cf proposal P0634: https://wg21.link/P0634
#define OPTIONAL_TYPENAME
#else
// They are still required in C++17
#define OPTIONAL_TYPENAME typename
#endif

#include <iostream>

struct TypeT {
  using V = int;
  using R = int;
  using C = std::vector<int>;
  using P = int *;
  using X = int;
  constexpr static void *X1 = nullptr;
  using X2 = int;
};

struct Dependent_x_is_a_value {
  constexpr static int x = 1;
};

struct Dependent_x_is_a_type {
  using x = int;
};
//#endregion

// OK, return type of a function declaration at global scope
template <class T> OPTIONAL_TYPENAME T::R f1();

template <class T> struct S {

  using V = OPTIONAL_TYPENAME T::V; // OK, in a defining-type-id

  OPTIONAL_TYPENAME T::R f(OPTIONAL_TYPENAME T::P p) { // OK, class scope
    return static_cast<OPTIONAL_TYPENAME T::R>(p); // OK, type-id of a static_cast
  }
  auto g() -> OPTIONAL_TYPENAME S<T *>::Ptr; // OK, trailing-return-type
};

template <class T> void f2() {
  void (*pf1)(T::X1);          // Meaningful typename which distinguishes
                               // variable of type void* initialized with T::X1
  void (*pf2)(typename T::X2); // and pointer to function T::X2 -> void
  //#region [Check types]
  std::cout << "type(pf1) is " << type(pf1) << std::endl;
  std::cout << "type(pf2) is " << type(pf2) << std::endl;
  //#endregion
}

template <class T>
void f3(typename T::R); // Would be ill-formed, declare a void variable
template <class T> void f4() {
  void g(typename T::X); // T::X at block scope does not denote a type
  //                     // (attempt to declare a void variable)
}

template <class T> void f5(int i) {
  T::x *i; // type or value ? (cf [temp.res]p6)
}

template <typename T> void f6(const std::vector<T> &v) {
  // std::vector<T>::const_iterator is a dependent name and assumed as a non-type
  typename std::vector<T>::const_iterator it = v.begin(); // typename required
  typename std::vector<T>::const_iterator typedef iter_t; // typename required
}

//#region [main]
int main() {
  f2<TypeT>();
  f5<Dependent_x_is_a_value>(1); // OK: T::x is a value
  // f5<Dependent_x_is_a_type>(1);  // error: dependent-name 'T::x' is parsed as a
  //                                //  non-type, but instantiation yields a type
}
//#endregion