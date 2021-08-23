#include <version>
#if !defined(__cpp_constexpr_dynamic_alloc) || !defined(__cpp_lib_to_array)
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_constexpr_dynamic_alloc), STR(__cpp_lib_to_array)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

//#region [GCC workaround]
// May be a bug in GCC
// cf: https://gcc.gnu.org/bugzilla/show_bug.cgi?id=93413
#ifdef __GNUC__
#ifndef __clang__
#if __GNUC__ <= 10 || __GNUC__ == 11 && __GNUC_MINOR__ <= 2
#define DISABLE_VIRTUAL_DTOR
#endif
#endif
#endif
//#endregion

#include <array>
#include <numeric>

//#region [details]
namespace custom {
namespace details {
double constexpr sqrtNewtonRaphson(double x, double curr, double prev) {
  return curr == prev ? curr : sqrtNewtonRaphson(x, 0.5 * (curr + x / curr), curr);
}
} // namespace details

/* Constexpr version of the square root
 * Return value:
 * - For a finite and non-negative value of "x",
 *   returns an approximation for the square root of "x"
 * - Otherwise, returns NaN
 */
double constexpr sqrt(double x) {
  return x >= 0 && x < std::numeric_limits<double>::infinity()
             ? details::sqrtNewtonRaphson(x, x, 0)
             : std::numeric_limits<double>::quiet_NaN();
}
} // namespace custom

struct Vector { // Custom aggregate
  double *_data;
  constexpr double &operator[](std::size_t i) noexcept { // constexpr required
    return _data[i];                                     //  for any call
  }                                                      // in constexpr context

  constexpr ~Vector() { // the compiler checks that
    delete[] _data;     // allocated memory is deallocated
  }
};

struct IOp { // Virtual method allowed !!
  virtual constexpr void apply(double &a, const double &b) const noexcept = 0;
#ifndef DISABLE_VIRTUAL_DTOR // GCC 11.2 workaround
  virtual constexpr ~IOp() = default;
#endif
};

struct AddOp : IOp {
  constexpr void apply(double &a, const double &b) const noexcept override {
    a += b;
  }
#ifndef DISABLE_VIRTUAL_DTOR // GCC 11.2 workaround
  constexpr ~AddOp() override = default;
#endif
};

struct SubOp : IOp {
  constexpr void apply(double &a, const double &b) const noexcept override {
    a -= b;
  }
#ifndef DISABLE_VIRTUAL_DTOR // GCC 11.2 workaround
  constexpr ~SubOp() override = default;
#endif
};
//#endregion

constexpr double f(double x) {
  // Variable in constexpr are restricted to literals and aggregates
  Vector v{new double[3]{x, x}}; // compile-time memory allocation
  // Vector v_not_set;           // error: data initialization REQUIRED by compiler
  const std::array a = std::to_array({3, 4, 5}); // aggregate OK

  IOp *op = new AddOp{}; // virtual class

  for (std::size_t i = 0; i < 3; ++i) {
    // a[i] = 0; // error: const rules are still applicable in constexpr
    op->apply(v[i], a[i]);
  }
  auto r = v[0]; // mutable object are allowed
  r += v[1] + v[2];
  // r += v[3]; // error: compiler has boundary checks

  delete op; //                // memory deallocation checked by compiler
  // delete op;                // error: double free checked by compiler
  return 1. / custom::sqrt(r); // floating point computation
                               // NB: std::sqrt is not _yet_ constexpr
}

#include <iostream>
int main() {
  static_assert(f(2.) == 0.25); // floating point assertion
  std::cout << f(2.) << '\n';
}

#endif