//#region [Collapse all]
#include <concepts>
#include <limits>
static constexpr double NaN = std::numeric_limits<double>::quiet_NaN();

int main() {
  static_assert(std::totally_ordered<double>);
  static_assert(2.0 < 3.4);
  static_assert(2.0 < NaN);  // assertion failed
  static_assert(-NaN < NaN); // assertion failed
  static_assert(-NaN < 2.0); // assertion failed
}
//#endregion