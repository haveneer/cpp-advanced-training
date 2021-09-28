#include <version>
#if !defined(__cpp_concepts) || !defined(__cpp_nontype_template_args) || \
    __cpp_nontype_template_args < 201911L
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_concepts), STR(__cpp_nontype_template_args) "_201911L"});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <concepts>
#include <iostream>
#include <type_traits>

// Non Type Template Parameter (NTTP)
// * https://en.cppreference.com/w/cpp/language/template_parameters
// C++20 : Class Types in Non-Type Template Parameters:
// * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1907r0.html

struct ConcreteDims {
  int i1 = 0;
  int i2 = 0;
  int i3 = 0;
  int i4 = 0;
};

constexpr struct DimsT {
  constexpr auto operator()(int i1, int i2) const { return ConcreteDims{i1, i2}; }
} Dims;

std::ostream &operator<<(std::ostream &o, const ConcreteDims &dims) {
  return o << "ConcreteDims(" << dims.i1 << "," << dims.i2 << "," << dims.i3 << ","
           << dims.i4 << ")";
}

std::ostream &operator<<(std::ostream &o, const DimsT &dims) {
  return o << "VirtualDims()";
}

template <typename T, auto Dims = Dims> struct array {
  array() requires(std::is_same_v<ConcreteDims, std::decay_t<decltype(Dims)>>) {
    std::cout << "Statically sized with dims = " << Dims << "\n";
  }
  array(ConcreteDims dims) requires(
      std::is_same_v<DimsT, std::decay_t<decltype(Dims)>>) {
    std::cout << "Dynamically sized with dims = " << dims << "\n";
  }
};

int main() {
  array<float, Dims(1, 2)> a; // static since Dims(1, 2) is constexpr
  // array<float, Dims(1, 2)> a{Dims(x,y)}; // invalid: compile time sizes
  array<float> b{Dims(1, 2)}; // dynamic since Dims is not constexpr
  // array<float> b{}; // invalid: runtime sizes required
}

#endif