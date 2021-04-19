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

#include <type_traits>
#include <vector>

template <int i>
concept GoodNumber = (i > 2) && (i < 10);

template <auto i>
concept GoodNumberOnAutoTemplate = (i > 2) && (i < 10);

template <typename... Args>
concept FoldExpression = requires(Args... args) {
  (args + ...); // parenthesis are still required
};

template <template <typename> class Container>
concept TemplateContainerShouldBeTemplateTemplate = requires {
  typename Container<int>::value_type;
  Container<int>::value_type; // complete instanciation: no ambiguity
};

template <typename T>
concept InstanciateAsManyAsYouWant = requires(T a, T b, T c, T d) {
  a + b / c ^ d;
};

template <typename T>
concept ComplexInstanciationIsAllowed = requires {
  std::vector<T>{new T[10]{.x = 2}};
};

int main() {}

#endif