#include <version>
#if !defined(__cpp_lib_chrono) || __cpp_lib_chrono < 201907L
//#region [Feature check]
#if __has_include("unsupported_features.hpp")
#include "unsupported_features.hpp"
REPORT_FEATURES({STR(__cpp_lib_chrono) "_201907L"});
#else
#error "Unsupported feature"
#endif
//#endregion
#else

#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

int main() {
  std::istringstream in{"Thu, 9 Jan 2014 12:35:34.123 +0000"};
  std::chrono::sys_time<std::chrono::seconds> tp;
  std::string format = "%a, %d %b %Y %T %z";
  in >> std::chrono::parse(format, tp);
  auto tp_days = floor<std::chrono::days>(tp);
  std::chrono::hh_mm_ss hms{tp - tp_days};
  //  std::cout << tp << '\n'; // should be OK in C++20 : http://wg21.link/p0355
  std::cout << "Number of days         = " << tp_days.time_since_epoch() << '\n';
  std::cout << "Number of hours        = " << hms.hours() << '\n';
  std::cout << "Number of minutes      = " << hms.minutes() << '\n';
  std::cout << "Number of seconds      = " << hms.seconds() << '\n';
  std::cout << "Number of milliseconds = " << hms.subseconds() << '\n';
}

#endif