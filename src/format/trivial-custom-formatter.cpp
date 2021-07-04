//#region [Collapse all]
#include <algorithm>
#include <cctype>
#include <iostream>
#include <string>
//#endregion

//#region [Compiler dependent <format> header]
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <version>
#ifdef __cpp_lib_format
#include <format>
#else
namespace std {
using namespace fmt;
}
#endif
//#endregion

// A wrapper for type T
template <class T> struct Box { T value; };

// Formatted using the format specification of the wrapped value
template <class T, class CharT>
struct std::formatter<Box<T>, CharT> : std::formatter<T, CharT> {
  // parse() is inherited from the base class

  // Define format() by calling the base class implementation with the wrapped value
  template <class FormatContext>                    // With C++20, you can replace
  auto format(const Box<T> &t, FormatContext &fc) { // template with auto arg
    return std::formatter<T, CharT>::format(t.value, fc);
  }
};

int main() {
  Box<int> v = {42};
  std::cout << std::format("{:#x}", v);
}