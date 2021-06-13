#include <version>
#if !defined(__cpp_concepts) || !defined(__cpp_lib_concepts)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_concepts)});
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

#include <concepts>
#include <iostream>
#include <mutex>
#include <utility>
#include <vector>

struct MyStruct {
  int data;
};

//#endregion

template <typename T>
concept swappable_as_member = requires(T x, T y) {
  x.swap(y);
};

template <typename T>
concept swappable_as_function = requires(T x, T y) {
  swap(x, y); // TODO: no std:: prefix; is-it a problem ?
  //#region [HINT]
  /* Concept definition are sensible to the current namespace
   * std::swap will never be used in that case.
   * cf: missing header, misspelling... */
  //#endregion
};

template <std::movable T> //
void best_swap(T &x, T &y) {
  //#region [Location allowed by GCC 10.3 & Clang 12 but not by MSVC 19.28]
  // constexpr bool has_member_swap = requires(T x, T y) { x.swap(y); };
  // constexpr bool has_nonmember_swap = requires(T x, T y) { swap(x, y); };
  //#endregion

  if constexpr (swappable_as_member<T>) { // use member swap
    std::cout << "Use member swap for T=" << type<T>() << '\n';
    x.swap(y);
  } else if constexpr (swappable_as_function<T>) { // use nonmember swap
    std::cout << "Use swap function for T=" << type<T>() << '\n';
    swap(x, y); // TODO again no std:: ?
  } else {
    std::cout << "Use general swap for T=" << type<T>() << '\n';
    T tmp = std::move(x);
    x = std::move(y);
    y = std::move(tmp);
  }
}

int main() {
  std::vector<int> v1, v2;
  best_swap(v1, v2);

  MyStruct s1, s2;
  best_swap(s1, s2);

  std::mutex m1, m2; // not movable
  // best_swap(m1, m2); // error: use of function 'void best_swap(T&, T&)
  //                    // [with T = std::mutex]' with unsatisfied constraints
}

#endif