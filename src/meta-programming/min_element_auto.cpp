// https://en.cppreference.com/w/cpp/language/function_template#Abbreviated_function_template
// auto min_element(auto first, auto last) {
#pragma message("not yet available everywhere and unknown feature tag")
auto min_element = [](auto first, auto last) {
  if (first == last)
    return last;

  auto smallest = first;
  ++first;
  for (; first != last; ++first) {
    if (*first < *smallest) {
      smallest = first;
    }
  }
  return smallest;
};

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