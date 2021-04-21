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
#include <string>
#include <vector>

template <int i>
concept GoodNumber = (i > 2) && (i < 10);
static_assert(GoodNumber<5> && !GoodNumber<10>); // You can assert it at compile-time

template <auto i>
concept GoodNumberOnAutoTemplate = (i > 60) && requires { (i < 70); }; // requires in
static_assert(GoodNumberOnAutoTemplate<'A'>);                          // conjunction

template <typename... Args>
concept FoldableExpression = requires(Args... args) { // Variadic constraint
  (args + ...);                                       // parenthesis are still required
};
static_assert(FoldableExpression<int, double> && !FoldableExpression<int, std::string>);

template <template <typename T, typename A> class Container> // You can make concept
concept CouldBeTemplateTemplate = requires {                 // template class
  typename Container<int, std::allocator<int>>::value_type;  // but constrained on
};                                                           // concrete type
static_assert(CouldBeTemplateTemplate<std::vector>);

template <typename T>
concept InstantiateAsManyAsYouWant = requires(T a, T b, T c, T d) {
  a + b / c ^ d;
};
static_assert(InstantiateAsManyAsYouWant<int> && !InstantiateAsManyAsYouWant<double>);

template <typename T>
concept ComplexInstanciationIsAllowed = requires {
  std::vector<T>{new T[10]{.x = 2}};
};

int main() {}

#endif