#include <version>
#ifndef __cpp_lib_coroutine
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_coroutine)});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <iostream>
#include <string>
#include <tuple>
#include <vector>

struct X {}; // FIXME TODO
auto read(const std::string &) { return X{}; }
auto parse(const X &) { return std::vector{std::make_tuple("", 0)}; }

int main() {
  std::string data_stream = "2 3 + ; 1 - 2 +";
  auto token_stream = read(data_stream);  // read is a coroutine
  auto expressions = parse(token_stream); // parse is a coroutine
  for (const auto &[frame, result] : expressions) {
    std::cout << frame << " = " << result << '\n';
  }
}

#endif