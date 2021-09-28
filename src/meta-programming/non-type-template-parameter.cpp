#include <version>
#if !defined(__cpp_nontype_template_args) || __cpp_nontype_template_args < 201911L
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_nontype_template_args) "_201911L"});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

// Non Type Template Parameter (NTTP)
// * https://en.cppreference.com/w/cpp/language/template_parameters
// C++20 : Class Types in Non-Type Template Parameters:
// * http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2019/p1907r0.html

template <double p> class NTTPOnFloat {};
NTTPOnFloat<3.14> x1{}; // (C++20) NTTP with floatting point value

template <int *p> class NTTPOnPointer {};
int a[10]; // global storage duration required
struct S {
  int m;
  static int s;
} s; // global storage duration required

NTTPOnPointer<&a[2]> x2; // error (until C++20): address of array element
NTTPOnPointer<&s.m> x3;  // error (until C++20): address of non-static member
NTTPOnPointer<&s.s> x4;  // ok: address of static member
NTTPOnPointer<&S::s> x5; // ok: address of static member

template <const int &CRI> struct B {};
// B<1> b2;// error: initializing 'const int&' with 'int' in converted constant
//         // expression does not bind directly
int c = 1; // global storage duration required
B<c> b1;   // OK!

int main() {}

#endif