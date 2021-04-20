#include <version>
#ifndef __cpp_concepts
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <cassert>
#include <iostream>
#include <utility>

//#region [isqrt without loop]
constexpr uint32_t clz(uint32_t x) {
  // clang-format off
  if (x == 0)
    return 32;
  uint32_t n = 0;
  if ((x & 0xFFFF0000) == 0) { n = n + 16; x <<= 16; }
  if ((x & 0xFF000000) == 0) { n = n + 8; x <<= 8; }
  if ((x & 0xF0000000) == 0) { n = n + 4; x <<= 4; }
  if ((x & 0xC0000000) == 0) { n = n + 2; x <<= 2; }
  if ((x & 0x80000000) == 0) { n = n + 1; } 
  return n;
  // clang-format on
}

constexpr uint32_t isqrt(uint32_t N) {
  if (N == 0)
    return 0;
  uint32_t xn = 1U << ((32 - clz(N)) / 2);
  xn = (xn + N / xn) / 2;
  xn = (xn + N / xn) / 2;
  xn = (xn + N / xn) / 2;
  return xn;
}
//#endregion

template <unsigned N> using range = std::make_integer_sequence<unsigned, isqrt(N)>;

// Concept version

template <unsigned N, unsigned M>
concept NotDivisible = (N % M != 0);

template <unsigned N, unsigned... Ms>
requires(NotDivisible<N, Ms + 2> &&...) //
struct ShiftAndCheck {
  ShiftAndCheck(std::integral_constant<unsigned, N>,
                std::integer_sequence<unsigned, Ms...>);
};

template <unsigned N>
concept IsPrime = requires(range<N> r) {
  ShiftAndCheck(std::integral_constant<unsigned, N>{}, r);
};

// C++17 version

template <unsigned N, typename Is> struct ShiftAndCheck2 {};
template <unsigned N, unsigned... Ms>
struct ShiftAndCheck2<N, std::integer_sequence<unsigned, Ms...>> {
  static constexpr bool value = ((N % (Ms + 2) != 0) && ...);
};

template <unsigned N> constexpr bool isPrime = ShiftAndCheck2<N, range<N>>::value;

int main() {
  // constexpr unsigned N = 17; // Only instantiable if N is prime
  // auto xx = ShiftAndCheck(std::integral_constant<unsigned, N>{}, range<N>{});

  std::cout << std::boolalpha;
  std::cout << "10 : " << IsPrime<10> << " " << isPrime<10> << '\n';
  std::cout << "11 : " << IsPrime<11> << " " << isPrime<11> << '\n';
  std::cout << "12 : " << IsPrime<12> << " " << isPrime<12> << '\n';
  std::cout << "13 : " << IsPrime<13> << " " << isPrime<13> << '\n';
  std::cout << "14 : " << IsPrime<14> << " " << isPrime<14> << '\n';
  std::cout << "15 : " << IsPrime<15> << " " << isPrime<15> << '\n';
  std::cout << "16 : " << IsPrime<16> << " " << isPrime<16> << '\n';
  std::cout << "17 : " << IsPrime<17> << " " << isPrime<17> << '\n';
  std::cout << "18 : " << IsPrime<18> << " " << isPrime<18> << '\n';
  std::cout << "19 : " << IsPrime<19> << " " << isPrime<19> << '\n';
}

#endif