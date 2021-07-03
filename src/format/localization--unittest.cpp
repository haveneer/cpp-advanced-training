//#region [Collapse all]
#include <chrono>
#include <gtest/gtest.h>
#include <iomanip>
#include <iostream>
#include <string_view>
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

//#region [Conditional constexpr]
#ifdef FMT_VERSION
#define FMT_CONSTEXPR constexpr
#else
#define FMT_CONSTEXPR
#endif
//#endregion

int main() {
  try {
    std::cout << "User-preferred locale setting is " << std::locale("").name()
              << '\n';
    // std::locale::global(std::locale(""));

    EXPECT_EQ("1024", std::format("{}", 1024));
    EXPECT_EQ("1024", std::format(std::locale{"en_US"}, "{}", 1024));
    EXPECT_EQ("1024", std::format(std::locale{"fr_FR"}, "{}", 1024));
#if 0
    using namespace std::chrono;
  auto ymd = 2000y / February / 28;
  auto datetime = std::chrono::sys_days(ymd);
#else
    std::tm tm = {};
    std::stringstream ss("Jun 25 2021 20:32");
    ss >> std::get_time(&tm, "%b %d %Y %H:%M");
    auto datetime = std::chrono::system_clock::from_time_t(std::mktime(&tm));
#endif
    EXPECT_EQ("2021-06-25 21:32:00", std::format("{}", datetime));
    EXPECT_EQ("2021-06-25T21:32:00+0200", std::format("{:%FT%T%z}", datetime));
    EXPECT_EQ("2021-06-25T21:32:00+0200",
              std::format(std::locale{"en_US"}, "{:%FT%T%z}", datetime));
    EXPECT_EQ("2021-06-25T21:32:00+0200",
              std::format(std::locale{"fr_FR"}, "{:%FT%T%z}", datetime));
    EXPECT_EQ("Fri Jun 25 21:32:00 2021",
              std::format(std::locale{"en_US"}, "{:%c}", datetime));
    EXPECT_EQ("Fri Jun 25 21:32:00 2021",
              std::format(std::locale{"fr_FR"}, "{:%c}", datetime));
  } catch (std::runtime_error &e) {
    std::cerr << "Locale error: " << e.what() << std::endl;
  }
  std::cout << "\n";

  EXPECT_EQ("😎⚒⚙️", std::format("😎⚒⚙️"));

  constexpr std::string_view localizations[] = {
      "The average of {0} and {1} is {2}.",            // Eng
      "{2:.6f} ist der Durchschnitt von {0} und {1}.", // De
      "La moyenne de {0} et {1} est {2}.",             // Fr
      "El promedio de {0} y {1} es {2}.",              // Es
      "{2} corrisponde alla media di {0} e {1}.",      // It
      "{0} 和 {1} 的平均值为 {2}。"                    // CN
  };

  FMT_CONSTEXPR int language = 2; // constexpr required by {fmt} // FIXME ?
  double a = 2.5;
  double b = 3.2;
  EXPECT_EQ("La moyenne de 2.5 et 3.2 est 2.85.",
            std::format(localizations[language], a, b, (a + b) / 2));

  struct my_number_punctuation : std::numpunct<char> { // en_US locale by hand
  protected:
    char do_decimal_point() const override { return ','; }
    std::string do_grouping() const override { return "\003"; }
    char do_thousands_sep() const override { return '\''; }
  };

  std::locale loc(std::locale(), new my_number_punctuation());

  auto oss_transform = [&loc](auto &&x) {
    std::ostringstream oss;
    oss.imbue(loc);
    oss << x;
    return oss.str();
  };

  EXPECT_EQ("1'234,57", oss_transform(1234.5678));
  EXPECT_EQ("1234.5678", std::format("{0:L}", 1234.5678));
  EXPECT_EQ("1234,5678", std::format(loc, "{0:L}", 1234.5678));
  EXPECT_EQ("1'234", std::format(loc, "{0:L}", 1234));
}