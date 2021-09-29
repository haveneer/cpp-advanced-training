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

template <size_t N> struct ConcreteDims {
  constexpr ConcreteDims(const int (&indexes)[N]) {
    for (size_t i = 0; i < N; ++i)
      this->indexes[i] = indexes[i];
  }
  int indexes[N];
};

template <size_t N> ConcreteDims(const int (&)[N]) -> ConcreteDims<N>;

constexpr struct NotConstrainedDims {
  template <auto...> constexpr auto operator()(auto... indexes) const {
    return ConcreteDims({indexes...});
  }
} Dims;

#ifdef my_cpp_feature_lambda_in_unevaluated_context
template <typename T>
concept IsConcreteType = requires(T **x) {
  // The double-pointer is necessary to avoid derived-to-base conversions, ex. struct
  // S : mytype<int, int, int> {}.
  []<size_t N>(ConcreteDims<N> **) {}(x); // unevaluated lambda
};
#else
template <typename T, template <auto...> class Z>
struct is_specialization_of : std::false_type {};

template <auto... Args, template <auto...> class Z>
struct is_specialization_of<Z<Args...>, Z> : std::true_type {};

template <typename T, template <auto...> class Z>
inline constexpr bool is_specialization_of_v = is_specialization_of<T, Z>::value;

template <typename T>
concept IsConcreteType = is_specialization_of_v<T, ConcreteDims>;
#endif
static_assert(IsConcreteType<ConcreteDims<2>>);
static_assert(not IsConcreteType<NotConstrainedDims>);

template <size_t N>
std::ostream &operator<<(std::ostream &o, const ConcreteDims<N> &dims) {
  o << "ConcreteDims(";
  for (int pos = 0; auto &&index : dims.indexes)
    o << ((pos++) ? "," : "") << index;
  o << ")";
  return o;
}

std::ostream &operator<<(std::ostream &o, const NotConstrainedDims &dims) {
  return o << "VirtualDims()";
}

template <typename T, auto Dims = Dims> struct array {
  using DimType = std::decay_t<decltype(Dims)>;
  array() requires(IsConcreteType<DimType>) {
    std::cout << "Statically sized with dims = " << Dims << "\n";
  }
  template <size_t N>
  array(ConcreteDims<N> dims) requires(std::is_same_v<NotConstrainedDims, DimType>) {
    std::cout << "Dynamically sized with dims = " << dims << "\n";
  }
};

int main() {
  array<float, Dims(1, 2)> a; // static since Dims(1, 2) is constexpr
  // array<float, Dims(1, 2)> a2{Dims(2,3)}; // invalid: compile time sizes
  array<float> b{Dims(1, 2)}; // dynamic since Dims is not constexpr
  // array<float> b2{}; // invalid: runtime sizes required
}

#endif