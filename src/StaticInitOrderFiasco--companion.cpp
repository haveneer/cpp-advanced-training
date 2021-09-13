//#region [Headers]
#include <cassert>
#include <type_traits>
//#endregion

// a runtime function: constexpr does not change anything
constexpr int square(int n) {
  assert(!std::is_constant_evaluated());
  return n * n;
}

auto staticCompanion = square(5); // a static data in a compilation unit