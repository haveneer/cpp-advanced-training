#include <iostream>
#include <string>
#include <string_view>

int main() {
  std::string_view sv = std::string("dangling");
  // warning: object backing the pointer will be destroyed at the end of the
  // full-expression [-Wdangling-gsl]
  
  std::cout << sv;
}