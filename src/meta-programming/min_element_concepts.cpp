#include <version>
#ifndef __cpp_concepts
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT(__FILE__, {STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <utility>
//#include <concepts>

template <typename T>
concept Comparable = requires(T a, T b) {
  a < b;
};

template <typename Iter>
concept ForwardIterator = requires(Iter iter) {
  ++iter;
  *iter;
};

template <ForwardIterator Iter> using value_type = decltype(*std::declval<Iter>());

template <ForwardIterator IterT>
IterT min_element(IterT first, IterT last) requires Comparable<value_type<IterT>> &&
    requires(value_type<IterT> &a, value_type<IterT> b) {
  {a = b};
  {a = std::move(b)};
}
{
  if (first == last)
    return last;

  IterT smallest = first;
  ++first;
  for (; first != last; ++first) {
    if (*first < *smallest) {
      smallest = first;
    }
  }
  return smallest;
}

#include <iostream>
#include <list>

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