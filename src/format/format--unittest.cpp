//#region [Collapse all]
#include <gtest/gtest.h>
#include <iostream>
#define CHECK(a, b)                \
  {                                \
    std::cout << (b) << std::endl; \
    EXPECT_EQ((a), (b));           \
  }
//#endregion

//#region [Compiler dependent format header]
#include <fmt/core.h>
#include <version>
#ifdef __cpp_lib_format
#include <format>
#else
namespace std { using namespace fmt; }
#endif
//#endregion

TEST(stdformat, trivial) {
  //
  CHECK("The answer is 42.", std::format("The answer is {}.", 42));
}

#include <fmt/color.h>
TEST(fmtlib, trivial) {
  fmt::print(fmt::emphasis::underline, "{} {}!", "Hello", "world");
}
