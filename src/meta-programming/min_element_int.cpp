int *min_element(int *first, int *last) {
  if (first == last)
    return last;

  int *smallest = first;
  ++first;
  for (; first != last; ++first) {
    if (*first < *smallest) {
      smallest = first;
    }
  }
  return smallest;
}

#include <array>
#include <iostream>

int main() {
  {
    int v[] = {5, 4, 1, 8, 4};
    auto minp = ::min_element(v, v + std::size(v));
    std::cout << "min value is " << *minp << '\n';
  }
}