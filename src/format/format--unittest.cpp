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
  // clang-format off

  EXPECT_EQ(std::format("{} + {} == {}", 1, 2, 3), "1 + 2 == 3");

  EXPECT_EQ(std::format("One is {1}. Zero is {0}.", 0, 1), "One is 1. Zero is 0.");

  EXPECT_EQ(std::format("{{}} is the natural placeholder"), "{} is the natural placeholder");

  // clang-format on
}

TEST(stdformat, specifier) {

  EXPECT_EQ("Hello", std::format("{}", "Hello"));
  EXPECT_EQ("Hello", std::format("{:}", "Hello"));
  EXPECT_EQ("Hello", std::format("{0:}", "Hello"));

  EXPECT_EQ("**+1.000**", std::format("{:*^+#10.4g}", 1.));

  // intentional blank line
}

TEST(stdformat, width_specifier) {
  EXPECT_EQ("    42", std::format("{:6}", 42));
  EXPECT_EQ("    42", std::format("{:{}}", 42, 6));
  EXPECT_EQ("    42", std::format("{1:{0}}", 6, 42));

  EXPECT_EQ("LongField", std::format("{}", "LongField"));
  EXPECT_EQ("LongField", std::format("{:6}", "LongField"));
  EXPECT_EQ("LongField   ", std::format("{:12}", "LongField"));
}

TEST(stdformat, fillalign_specifier) {
  EXPECT_EQ("    42", std::format("{:6}", 42));
  EXPECT_EQ("42    ", std::format("{:<6}", 42));
  EXPECT_EQ("x     ", std::format("{:6}", 'x'));
  EXPECT_EQ("x*****", std::format("{:*<6}", 'x'));
  EXPECT_EQ("*****x", std::format("{:*>6}", 'x'));
  EXPECT_EQ("**x***", std::format("{:*^6}", 'x'));
  EXPECT_EQ("**42**", std::format("{:*^6}", 42));
}

TEST(stdformat, type_specifier) {
  void *ptr = reinterpret_cast<void *>(0x7f880d407020);
  // clang-format off
  EXPECT_EQ("42 101010 52 42 2a",                                 std::format("{0} {0:b} {0:o} {0:d} {0:x}", 42));
  EXPECT_EQ("3.1 3.100000e+00 3.100000 3.1 0x1.8cccccccccccdp+1", std::format("{0} {0:e} {0:f} {0:g} {0:a}", 3.1));
  EXPECT_EQ("true true 1",                                        std::format("{0} {0:s} {0:d}", true));
  EXPECT_EQ("A A 1000001 65 41",                                  std::format("{0} {0:c} {0:b} {0:d} {0:x}", 'A'));
  EXPECT_EQ("Hello World",                                        std::format("{} {:s}", "Hello", "World"));
  EXPECT_EQ("0x7f880d407020 0x7f880d407020",                      std::format("{0} {0:p}", ptr));
  // clang-format on
}

TEST(stdformat, prefix_specifier) {
  auto inf = std::numeric_limits<double>::infinity();
  auto nan = std::numeric_limits<double>::quiet_NaN();
  // clang-format off
  EXPECT_EQ("1|+1|1| 1",         std::format("{0:}|{0:+}|{0:-}|{0: }", 1));
  EXPECT_EQ("-1|-1|-1|-1",       std::format("{0:}|{0:+}|{0:-}|{0: }", -1));
  EXPECT_EQ("inf|+inf|inf| inf", std::format("{0:}|{0:+}|{0:-}|{0: }", inf));
  EXPECT_EQ("nan|+nan|nan| nan", std::format("{0:}|{0:+}|{0:-}|{0: }", nan));

  EXPECT_EQ("+00065", std::format("{:+06d}", 'A'));
  EXPECT_EQ("   3.0", std::format("{:#6}", 3.));
  EXPECT_EQ("0x000a", std::format("{:#06x}", 0xa));
  // clang-format on
}

TEST(stdformat, precision_specifier) {
  auto x = (1 + std::sqrt(5)) / 2;
  // clang-format off
  EXPECT_EQ("2",             std::format("{:.1}", x));
  EXPECT_EQ("1.6",           std::format("{:.2}", x));
  EXPECT_EQ("1.6180e+00",    std::format("{:.4e}", x));
  EXPECT_EQ("1.6180",        std::format("{:.4f}", x));
  EXPECT_EQ("1.618",         std::format("{:.4}", x)); // same of {:.4g}
  EXPECT_EQ("1.61803399",    std::format("{:.{}g}", x, 9));

  EXPECT_EQ("Hello folks",   std::format("{:.5} folks", "Hello World"));
  EXPECT_EQ("Hello World !", std::format("{:.20} !", "Hello World"));
  EXPECT_EQ("Hellfest",      std::format("{1:.{0}}fest", 4, "Hello World"));
  // clang-format on
}

TEST(stdformat, chrono) {
  using namespace std::literals::chrono_literals;
  // https://en.cppreference.com/w/cpp/chrono/system_clock/formatter
  // clang-format off
  EXPECT_EQ("Default format: 42s 100ms", std::format("Default format: {} {}", 42s, 100ms));
  EXPECT_EQ("Conversion: 86000s == 23:53:20", std::format("Conversion: {0} == {0:%H:%M:%S}", 86000s));
  EXPECT_EQ("strftime-like format: 03:15:30", std::format("strftime-like format: {:%H:%M:%S}", 3h + 15min + 30s));

//  using namespace std::chrono;
//  auto ymd = 2000y / February / 28;
//  auto datetime = std::chrono::sys_days(ymd);
//  EXPECT_EQ("2000-02-28 01:00:00", std::format("{}", datetime));
//  EXPECT_EQ("2000-02-28T01:00:00+0100", std::format("{:%FT%T%z}", datetime));
  // clang-format on
}

#include <fmt/color.h>
TEST(fmtlib, trivial) {
  fmt::print(fmt::emphasis::underline, "{} {}!", "Hello", "world");
}
