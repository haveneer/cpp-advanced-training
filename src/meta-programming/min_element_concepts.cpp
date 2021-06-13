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
#include <concepts>
#include <iostream>
#include <list>
#include <utility>
//#endregion

template <typename Iter>
concept ForwardIterator = requires(Iter iter) {
  ++iter;
  *iter;
};

template <typename T>
concept Comparable = requires(T a, T b) {
  { a < b } -> std::same_as<bool>;
};

template <typename T>
concept Copiable = requires(T a, T b) {
  {a = b};
  {a = std::move(b)};
};

template <ForwardIterator I> using deref_t = decltype(*std::declval<I>());
template <typename I>
concept ValuableIterator = Comparable<deref_t<I>> &&(Copiable<deref_t<I>>);

template <ForwardIterator IterT>
IterT min_element(IterT first, IterT last) requires ValuableIterator<IterT> {
  if (first == last) {
    return last;
  }

  IterT smallest = first;
  ++first;
  for (; first != last; ++first) {
    if (*first < *smallest) {
      smallest = first;
    }
  }
  return smallest;
}

int main() {
  {
    int v[] = {5, 4, 1, 8, 4};
    auto minp = ::min_element(std::begin(v), std::end(v));
    std::cout << "min value is " << *minp << '\n';
  }

  {
    std::list<double> v = {{5.1, 4.1, 1.1, 8.1, 4.1}};
    auto minp = ::min_element(std::begin(v), std::end(v));
    std::cout << "min value is " << *minp << '\n';
  }
}

#endif