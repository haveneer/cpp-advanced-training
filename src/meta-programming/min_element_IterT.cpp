//#region [Collapse all]
#include <iostream>
#include <list>
//#endregion

template <typename IterT> // IterT could be int*, double* or any iterator ...
IterT min_element(IterT first, IterT last) {
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
}