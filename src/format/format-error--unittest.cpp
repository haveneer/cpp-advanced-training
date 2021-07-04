//#region [Collapse all]
#include <chrono>
#include <cmath>
#include <gtest/gtest.h>
#include <iostream>
// Reserve #include (to avoid to recompute code slice)
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

TEST(stdformat, trivial) {
#ifndef FMT_VERSION
  // {fmt} lib checks these errors at compile time (=> new proposal P2216)
  EXPECT_THROW(std::format("{:.}", 42), std::format_error);
  EXPECT_THROW(std::format("{:.2}", 'a'), std::format_error);
  EXPECT_THROW(std::format("{:00}", "string"), std::format_error);
  EXPECT_THROW(std::format("{:-5}", "string"), std::format_error);
  EXPECT_THROW(std::format("{", "string"), std::format_error);
  EXPECT_THROW(std::format("{1}", "string"), std::format_error);
#endif
  
  EXPECT_THROW(std::format("{0:{1}}", "string", -1), std::format_error);
  EXPECT_THROW(std::format("{:.{}}", 1.2, -5), std::format_error);
}
