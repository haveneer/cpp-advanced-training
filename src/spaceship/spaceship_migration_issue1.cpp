#include <version>
#include <cassert>

struct A {
  bool operator==(int) /* no const */ { return false; }
  friend bool operator==(int, const A &) { return true; }
};

struct B {
  bool operator==(int) const { return false; }
  friend bool operator==(int, const B &) { return true; }
};

int main() {
  {
    A a{};
    const bool resa1 = (a == 4);
    const bool resa2 = (4 == a);
#if __cplusplus > 201703L && defined(__cpp_impl_three_way_comparison) // C++ ≥20
    assert(resa1 == false); // resa1 == a.operator==(4);
    assert(resa2 == false); // resa2 == a.operator==(4);
#else                       // C++ <20
    assert(resa1 == false); // resa1 == a.operator==(4);
    assert(resa2 == true);  // resa2 == operator==(4, a);
#endif
  }
  {
    B b{};
    const bool resb1 = (b == 4);
    const bool resb2 = (4 == b);
    // C++20 & C++17 : Why ?
    assert(resb1 == false); // resb1 == b.operator==(4);
    assert(resb2 == true);  // resb2 == operator==(4, b);
  }
}