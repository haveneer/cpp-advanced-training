//#region [Collapse all]
#include <array>
#include <iostream>
//#endregion

double *min_element(double *first, double *last) {
  if (first == last)
    return last;

  double *smallest = first;
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
    double v[] = {5.1, 4.1, 1.1, 8.1, 4.1};
    auto minp = ::min_element(v, v + std::size(v));
    std::cout << "min value is " << *minp << '\n';
  }
}