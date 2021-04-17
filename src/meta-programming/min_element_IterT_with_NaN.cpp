template <typename IterT> IterT min_element(IterT first, IterT last) {
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
#include <limits>
#include <list>
#include <vector>
static constexpr double NaN = std::numeric_limits<double>::quiet_NaN();

int main() {
  {
    std::vector<double> v = {{NaN, 4.1, 1.1, 8.1, 4.1}};
    auto minp = ::min_element(std::begin(v), std::end(v));
    std::cout << "min value is " << *minp << '\n';
  }
  
  {
    std::vector<double> v = {{5.1, NaN, 1.1, 8.1, 4.1}};
    auto minp = ::min_element(std::begin(v), std::end(v));
    std::cout << "min value is " << *minp << '\n';
  }
}