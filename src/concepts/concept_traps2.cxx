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

#include <concepts>

// Declarations
struct S {
  template <typename Number>
  requires std::integral<Number>
  void f(Number n);
};
//#region [HINT Answer]
// The location of the constraint matters !
// Always choose the same location across all your declarations/definitions
// trailing definition is different from head definition
// #A template <typename Number>  requires std::integral<Number> void f(Number n);
// #B template <std::integral Number> void f(Number n);
// #C template <typename Number> void f(Number n);
// #A <=> #B but <!=> #C
//#endregion

// Definitions
// clang-format off
template <typename Number> 
void S::f(Number n)                // error: ambiguating new declaration 
requires std::integral<Number> { } // of 'void S::f(Number) requires  integral<Number>'
// clang-format on

int main() {
  S s;
  s.f(1);
}

#endif