//#region [Collapse all]
#include <iostream>
#include <list>
//#endregion

template <typename IterT> //
IterT min_element(IterT first, IterT last) {
  if (first == last)
    return last;

  IterT smallest = first;
  ++first;
  for (; first != last; ++first) {
    if (*first < *smallest) { // error: invalid type argument of unary '*'
      smallest = first;       // (have 'int')
    }
  }
  return smallest;
}

int main() {
  {
    auto minp = ::min_element(3, 10);     // IterT is 'int'
    std::cout << "min value is " << *minp // error: invalid type argument
              << '\n';                    // of unary '*' (have 'int')
  }

  {
    int v[] = {5, 4, 1, 8, 4};
    auto minp = ::min_element(&v, &v + 5);         // IterT is 'int**'
    std::cout << "min value is " << *minp << '\n'; // compiles and runs but 'strange'
  }                                                // Is it what you expect ?
}