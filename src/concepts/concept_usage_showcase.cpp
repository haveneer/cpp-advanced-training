#include <version>
#if !defined(my_cpp_abbreviated_function_template) || !defined(__cpp_concepts)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(my_cpp_abbreviated_function_template), STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

// MyConcept definition
template <typename T>
concept MyConcept = requires {
  true;
};

// MyConcept usages
template <MyConcept T> struct S1 {}; // #1

template <MyConcept T> T f1(const T &t) { return t; } // #1

template <typename T>
requires MyConcept<T>
struct S2 {
}; // #2

template <typename T> T f2(const T &t) requires MyConcept<T> { return t; } // #2a trailing form

template <typename T>
requires MyConcept<T> T f2b(const T &t) { return t; } // #2b (two allowed locations)

auto f3(const MyConcept auto &t) { return t; } // #3
// auto f3b(const MyConcept &t) { ... } // error: expected 'auto' or 'decltype(auto)'

MyConcept auto f4(const MyConcept auto &t) { return t; } // #3

auto f5(const auto &t) requires MyConcept<decltype(t)> {} // #2

#include <initializer_list> // required for for-range loop on { ... }
int main() {
  MyConcept auto v = 1; // #4
  // MyConcept v = 1; // error: expected 'auto' or 'decltype(auto)' after 'MyConcept'

  for (MyConcept auto &&i : {1, 2, 3, 4}) { // #4
    static_assert(MyConcept<decltype(i)>);
  }
  {
    f1(v);
    f2(v);
    f3(v);
    f4(v);
    f5(v);
  } // if you don't instantiate them, you can know if they are valid
}

#endif