//#region [Declarations]
#include <iostream>
#include <tuple>
#include <type_traits>
//#endregion

// A simple constexpr function
constexpr std::tuple<int, bool> factorial(int n) noexcept {
  // Note: When directly used as the condition of static_assert declaration or
  // constexpr if statement, std::is_constant_evaluated() always returns true.
  return {n < 2 ? 1 : n * std::get<0>(factorial(n - 1)),
          std::is_constant_evaluated()};
}

int main() {
  auto [f5, b1] = factorial(5);
  std::cout << "5! = " << f5 << " compile-time=" << b1 << '\n';

  int x = std::rand() % 20;
  auto [frand, b2] = factorial(x);
  std::cout << x << "! = " << frand << " compile-time=" << b2 << '\n';
}