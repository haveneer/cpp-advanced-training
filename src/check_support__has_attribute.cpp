//#region [declarations]
#include <iostream>
//#endregion

#include <version>

#if __cpp_lib_optional
#include <optional>
#define HAS_OPTIONAL
#endif

int main() {
#ifdef HAS_OPTIONAL
  std::optional<int> value; // = 1;
  if (value.has_value()) {
    std::cout << "'value' contains " << value.value() << '\n';
  } else {
    std::puts("'value' does not contains any value");
  }
#else
  std::puts("std::optional feature is not available");
#endif
}