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
//#region [Collapse all]
#include <string>
#include <vector>
#include <utility>
//#endregion

template <int i>
concept GoodNumber = (i > 2) && (i < 10);
static_assert(GoodNumber<5> && !GoodNumber<10>); // You can assert it at compile-time

template <auto i>
concept GoodNumberOnAutoTemplate = (i > 60) && requires { (i < 70); }; // requires in
static_assert(GoodNumberOnAutoTemplate<'A'>);
static_assert(GoodNumberOnAutoTemplate<75LLU>); // TODO Why is it OK ?

template <typename... Args>
concept FoldableExpression = requires(Args... args) { // Variadic constraint
  (args + ...);                                       // parenthesis are still required
};
static_assert(FoldableExpression<int, double>);
static_assert(!FoldableExpression<int, std::string>);

template <template <typename T, typename A> class Container> // You can make concept
concept CouldBeTemplateTemplate = requires {                 // template class
  typename Container<int, std::allocator<int>>::value_type;  // but constrained on
};                                                           // concrete type
static_assert(CouldBeTemplateTemplate<std::vector>);
static_assert(!CouldBeTemplateTemplate<std::pair>);

template <typename T>
concept InstantiateAsManyAsYouWant = requires(T a, T b, T c, T d) {
  a + b / c ^ d;
};
//#region [How to check a large set of type ?]
// C++17 style
template<typename... Ts>
constexpr bool check() { return (InstantiateAsManyAsYouWant<Ts> && ...); }
static_assert(check<int, unsigned, char, signed, long>());
// C++20 template lambda style
#if defined(__cpp_generic_lambdas) && __cpp_generic_lambdas >=201707L 
static_assert([]<typename... Ts> { // using template lambdas
  return (!InstantiateAsManyAsYouWant<Ts> && ...);
}.template operator()<std::string, double, std::byte>());
#endif
//#endregion

template <typename T>
concept ComplexInstanciationIsAllowed = requires {
  std::vector<T*>{new T{.x = 2}};
};
//#region [Ad-hoc structs for testing]
struct Complex { int x; };
static_assert(ComplexInstanciationIsAllowed<Complex>);
static_assert(!ComplexInstanciationIsAllowed<double>);

int main() {}
//#endregion

#endif