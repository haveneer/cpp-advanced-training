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

#include <concepts>
#include <utility>
template <std::movable T> //
void smart_swap(T &x, T &y) {
  constexpr bool has_member_swap = requires(T x, T y) { x.swap(y); };
  constexpr bool has_nonmember_swap = requires(T x, T y) { swap(x, y); };
  if constexpr (has_member_swap) { // use member swap
    x.swap(y);
  }
  else if constexpr (has_nonmember_swap) { // use nonmember swap
    swap(x, y);
  }
  else {
    // use general swap algorithm
    T tmp = std::move(x);
    x = std::move(y);
    y = std::move(tmp);
  }
}

#include <mutex>
#include <vector>

struct S {
  int data;
};

int main() {
  std::vector<int> v1, v2;
  smart_swap(v1, v2);

  S s1, s2;
  smart_swap(s1, s2);

  std::mutex m1, m2; // not movable
  // smart_swap(m1, m2); // error: use of function 'void smart_swap(T&, T&)
  //                     // [with T = std::mutex]' with unsatisfied constraints
}

#endif