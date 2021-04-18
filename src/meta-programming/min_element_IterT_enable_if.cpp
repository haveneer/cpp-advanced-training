//#region [Collapse all]
#include <iostream>
#include <list>
#include <type_traits>
//#endregion

// #1 : using enable_if on return type
template <typename IterT>
std::enable_if_t<std::is_void_v<decltype(*std::declval<IterT &>(),
                                         ++std::declval<IterT &>(), void())>,
                 IterT>
min_element(IterT first, IterT last) {
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

// #2 using manual SFINAE
template <typename IterT, typename = decltype(*std::declval<IterT &>(),
                                              ++std::declval<IterT &>(), void())>
IterT min_element2(IterT first, IterT last) {
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

// #3 using enable_if with optional template argument trick
template <
    typename IterT,
    std::enable_if_t<std::is_void_v<decltype(*std::declval<IterT &>(),
                                             ++std::declval<IterT &>(), void())>,
                     int> = 0>
IterT min_element3(IterT first, IterT last) {
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

  // error: no matching function for call to 'min_element(int, int)'
  // auto minp = ::min_element(3, 10);

  // bypass security of min_element2
  // ::min_element2<int, void>(3, 10);
}