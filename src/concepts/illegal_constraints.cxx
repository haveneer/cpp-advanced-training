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

template <typename T>
concept InvalidSyntax = requires(T t) {
  t+++; // error: expected primary-expression before ';' token
};

template <typename T>
concept MultipleStatementCompound = requires(T t) {
  { t++; ++t }; // error: expected '}' before ';' token
};

template <typename T>
concept ConditionalNoExceptIsNotAllowed = requires { // #3
  { T{T{}} } noexcept(true); // error: expected ';' before '(' token
};

template <typename T>
concept TypeRequirementShouldBeAConstraint = requires(T a, T b) {
  { a < b } -> bool; // error: return-type-requirement is not a type-constraint
};

template <typename Container, typename T>
concept TemplateContainerShouldBeTemplateTemplate = requires {
  typename Container<T>; // error: expected ';' before '<' token
};

template <typename T>
concept DoNotCreateLocalVariable = requires(T a) {
  T b = a; // error: expected primary-expression before 'b'
};

int main() {}

#endif