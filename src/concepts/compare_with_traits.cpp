#include <version>
#if !(defined(__cpp_concepts) && defined(__cpp_lib_concepts))
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_lib_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else
// clang-format off

//#region [Headers]
#include <concepts>
#include <type_traits>
//#endregion

template <class T>
concept MyConcept = std::integral<T>;                             // OK concept
template <class T>
concept MyConcept2 = std::is_integral_v<T>;                       // OK trait
static_assert(MyConcept<int> == MyConcept2<int>);                 // OK both

template <std::integral T> void f1() {}                           // OK concept
template <typename T> requires std::integral<T> void f2() {}      // OK trait

template <std::is_integral T> void f3() {} // error: only if instantiated
template <typename T> requires std::is_integral_v<T> void f4() {} // OK concept only

int main() {
  f1<int>();    /* f1<double>(); // error: unsatisfied constraints */
  f2<int>();    /* f2<double>(); // error: unsatisfied constraints */
  // f3<int>(); /* f3<double>(); // error: no matching function for call to 'f3<double>() */
  f4<int>();    /* f4<double>(); // error: unsatisfied constraints */
}

#endif