#include <cassert>
#include <version>

struct B;

struct A {
  bool operator==(/* const */ B &) const { return true; }
};

struct B {
  bool operator==(const A &) const { return false; }
};

int main() {
  A a{};
  B b{};
  const bool res1 = (a == b);
  const bool res2 = (b == a);
#if __cplusplus > 201703L && defined(__cpp_impl_three_way_comparison) // C++ ≥20
  assert(res1 == true); // res1 == a.operator==(4);
  assert(res2 == true); // res2 == a.operator==(4);
#else                   // C++ <20
  assert(res1 == true);  // res1 == a.operator==(4);
  assert(res2 == false); // res2 == operator==(4, a);
#endif
}
//
//
//
//
//
//
// Comment for code alignment
//   as spaceship_migration_issue1.cpp
//