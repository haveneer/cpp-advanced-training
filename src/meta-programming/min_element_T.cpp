//#region [Collapse all]
#include <array>
#include <iostream>
//#endregion

template <typename T> // T could be int, double...
T *min_element(T *first, T *last) {
  if (first == last)
    return last;

  T *smallest = first;
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
    auto minp = ::min_element(v, v + std::size(v));
    std::cout << "min value is " << *minp << '\n';
  }

  {
    double v[] = {5.1, 4.1, 1.1, 8.1, 4.1};
    auto minp = ::min_element(v, v + std::size(v));
    std::cout << "min value is " << *minp << '\n';
  }
}