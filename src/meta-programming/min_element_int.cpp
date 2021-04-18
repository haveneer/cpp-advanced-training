//#region [Collapse all]
#include <array>
#include <iostream>
//#endregion

int *min_element(int *first, int *last) {
  if (first == last) // we assume an close-open interval [first, last[
    return last;     // empty interval case

  int *smallest = first;           // first candidate
  ++first;                         //
  for (; first != last; ++first) { // for next candidates
    if (*first < *smallest) {      // is this other candidate better ?
      smallest = first;            // if yes, we take its address
    }
  }
  return smallest; // we return the address of the 'best' candidate
}

int main() {
  {
    int v[] = {5, 4, 1, 8, 4};
    auto minp = ::min_element(v, v + std::size(v));
    std::cout << "min value is " << *minp << '\n';
  }
}