//#region [declarations]
#include <iostream>
#include <vector>
//#endregion

int main() {
  // generic lambda, operator() is a template with two parameters
  auto glambda = [](auto a, auto &&b) { return a < b; };
  bool b = glambda(3, 3.14); // ok

  // generic lambda, operator() is a template with one parameter
  auto vglambda = [](auto &&...ts) { // generic lambda, ts is a parameter pack
    std::cout << ">", ((std::cout << " " << std::forward<decltype(ts)>(ts)), ...)
                          << '\n';
  };
  vglambda(1, 'a', 3.14); // outputs "> 1 a 3.14"

  // generic lambda, operator() is a template with two parameters (C++20)
  auto glambdaT = []<typename T>(T a, T &&b) { return a < b; };
  // bool bT = glambdaT(3, 3.14); // error : both arguments do not have same type
  bool bT = glambdaT(3, 314); // OK

  // generic lambda, operator() is a template with one parameter pack (C++20)
  auto vglambdaT = []<typename... Ts>(Ts && ...ts) {
    std::cout << ">", ((std::cout << " " << std::forward<Ts>(ts)), ...) << '\n';
  };
  vglambdaT(1, 'a', 3.14); // outputs "> 1 a 3.14"
}