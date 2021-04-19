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
// clang-format off

#include <type_traits>

template <typename T1, typename T2>
concept Addable = requires(T1 &t1, T2 t2) { // #1
  t1 + t2;
  t1 += t2;
};

template <typename T>
concept HasValueType = requires { // #2
  typename T::value_type;
};

template <typename T>
concept NonThrowDefaultCtor = requires { // #3
  { T{} } noexcept;
};

template <typename T, typename U>
concept same_as = std::is_same<T, U>::value;

template <typename T>
concept Comparable = requires(T a, T b) { // #3
  { a < b } -> same_as<bool>;
  { a == b } -> same_as<bool>;
};

template <typename T>
concept Ringable = requires { // #4
  requires Addable<T, T> && Comparable<T>;
  requires requires(T t) {
    { t * t } -> same_as<T>;
  };
};

constexpr bool negate(bool b) { return !b; }
template <typename T, typename U> // not possible to be defined on same_as
concept not_same_as = negate(std::is_same<T, U>::value);

template <typename T>
concept Impossible = requires(T t) { // #3
  { t } -> not_same_as<T>;
};

template <typename T>
concept CatchMeIfYouCan = requires(T t) { // #1
  *~**&**!***&+**t++()|&T{"'"};
};

int main() {}

#endif