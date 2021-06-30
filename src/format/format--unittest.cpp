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
#if __has_include(<format>)
#include <format>
namespace stdx = std;
#else
#include <fmt/core.h>
namespace stdx = fmt;
#endif
//#endregion

TEST(format, canary) {
  //
  CHECK("The answer is 42.", stdx::format("The answer is {}.", 42));
}

#ifdef FMT_VERSION
#include <fmt/color.h>
TEST(format, fmtlib) {
  fmt::print(fmt::emphasis::underline, "{} {}!", "Hello", "world");
}
#endif