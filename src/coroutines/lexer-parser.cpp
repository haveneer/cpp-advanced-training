#include <version>
#if __has_include(<experimental/coroutine>)
#define __cpp_lib_coroutine
#endif
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

//#region [Collapse all]
#if __has_include(<experimental/coroutine>)
#include <experimental/coroutine>
namespace std {
using namespace std::experimental;
}
#else
#include <coroutine>
#endif
#include <iostream>
#include <string>
#include <tuple>
#include <vector>
//#endregion

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