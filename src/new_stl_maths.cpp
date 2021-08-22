#include <version>
#if !defined(__cpp_lib_math_constants) || !defined(__cpp_lib_bit_cast)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_math_constants), STR(__cpp_lib_bit_cast)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [Headers]
#include <iomanip>
#include <iostream>
#include <limits>
//#endregion
#include <bit>     // for std::bit_cast
#include <cmath>   // for std::lerp
#include <numbers> // for std::numbers
#include <numeric> // for std::midpoint

int main() {
  // clang-format off
  std::cout << std::setprecision(17);
  std::cout << std::setw(33) << "std::numbers::e: " << std::numbers::e << '\n';
  std::cout << std::setw(33) << "std::numbers::log2e: " << std::numbers::log2e << '\n';
  std::cout << std::setw(33) << "std::numbers::log10e: " << std::numbers::log10e << '\n';
  std::cout << std::setw(33) << "std::numbers::pi: " << std::numbers::pi << '\n';
  std::cout << std::setw(33) << "std::numbers::inv_pi: " << std::numbers::inv_pi << '\n';
  std::cout << std::setw(33) << "std::numbers::inv_sqrtpi: " << std::numbers::inv_sqrtpi << '\n';
  std::cout << std::setw(33) << "std::numbers::ln2: " << std::numbers::ln2 << '\n';
  std::cout << std::setw(33) << "std::numbers::sqrt2: " << std::numbers::sqrt2 << '\n';
  std::cout << std::setw(33) << "std::numbers::sqrt3: " << std::numbers::sqrt3 << '\n';
  std::cout << std::setw(33) << "std::numbers::inv_sqrt3: " << std::numbers::inv_sqrt3 << '\n';
  std::cout << std::setw(33) << "std::numbers::egamma: " << std::numbers::egamma << '\n';
  std::cout << std::setw(33) << "std::numbers::phi: " << std::numbers::phi << "\n\n";

  std::cout << std::setprecision(20);
  std::cout << std::setw(33) << "std::numbers::pi_v<double>: " << std::numbers::pi_v<double> << '\n';
  std::cout << std::setw(33) << "std::numbers::pi_v<long double>: " << std::numbers::pi_v<long double> << "\n\n";

  std::cout << std::setprecision(16);
  constexpr double f64v = 0.3;
  constexpr std::uint64_t u64v = 0x3cb0000000000000ULL;
  std::cout << "std::bit_cast<std::uint64_t>(" << f64v << ") == 0x" << std::hex << std::bit_cast<std::uint64_t>(f64v) << '\n';
  std::cout << "std::bit_cast<double>(0x" << std::hex << u64v << ") == " << std::bit_cast<double>(u64v) << '\n';
  // clang-format on
}

#endif