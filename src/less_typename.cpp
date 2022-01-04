//#region [Declarations]
#ifdef DEBUG_TYPE
#include "type.hpp" // advanced type printing
#else // clang-format off
#include <typeinfo>
template <typename T> std::string type() { if (std::is_same_v<std::remove_extent_t<std::remove_const_t< std::remove_pointer_t<std::remove_reference_t<T>>>>, T>) return typeid(T).name(); else if (std::is_array_v<T>) return type<std::remove_extent_t<T>>() + "[]"; else if (std::is_const_v<T>) return type<std::remove_const_t<T>>() + " const"; else if (std::is_pointer_v<T>) return type<std::remove_pointer_t<T>>() + "*"; else if (std::is_reference_v<T>) return type<std::remove_reference_t<T>>() + ((std::is_lvalue_reference_v<T>) ? "&" : "") + ((std::is_rvalue_reference_v<T>) ? "&&" : ""); else return std::string("cannot decode ") + typeid(T).name(); }
#endif              // clang-format on

#include <vector>
#if __cplusplus > 201703L
// C++20 compilers can infer non-ambiguous typename keyword
// Cf proposal P0634: https://wg21.link/P0634
#define OPTIONAL_TYPENAME
#else
// They are still required in C++17
#define OPTIONAL_TYPENAME typename
#endif
#define REQUIRED_TYPENAME typename
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
//#endregion

// OK, return type of a function declaration at global scope
template <class T> OPTIONAL_TYPENAME T::R f1();

template <class T> struct S : OPTIONAL_TYPENAME T::C { // OK, inheritance

  using V = OPTIONAL_TYPENAME T::V; // OK, in a defining-type-id

  OPTIONAL_TYPENAME T::R f(OPTIONAL_TYPENAME T::P p) { // OK, class scope
    return static_cast<OPTIONAL_TYPENAME T::R>(p); // OK, type-id of a static_cast
  }
  auto g() -> OPTIONAL_TYPENAME S<T *>::Ptr; // OK, trailing-return-type
};

template <class T> void f2() {
  void (*pf1)(T::X1);          // Meaningful typename which Distinguishes
                               // variable pf of type void* initialized with T::X and
  void (*pf2)(typename T::X2); // and pointer to function T::X2 -> void
  //#region [Check types]
  std::cout << "type(pf1) is " << type(pf1) << std::endl;
  std::cout << "type(pf2) is " << type(pf2) << std::endl;
  //#endregion
}

template <class T>
void f3(REQUIRED_TYPENAME T::R); // Would be ill-formed, declare a void variable
template <class T> void f4() {
  void g(REQUIRED_TYPENAME T::X); // T::X at block scope does not denote a type
  //                              // (attempt to declare a void variable)
}

//#region [main]
int main() { f2<TypeT>(); }
//#endregion