//#region [Collapse all]
#include <cassert>
#include <iomanip>
#include <iostream>
#if __has_include(<numbers>)
#include <numbers> // C++20
#else
namespace std::numbers {
constexpr double pi = 3.1415926535897932;
}
#endif
#include <sstream>
#include <string>
#include <tuple>
#include <vector>
//#endregion

int main() {
  std::vector<std::tuple<std::string, int, double, std::string>> data{
      {"John", 42, -std::numbers::pi,
       "Name:      John; answer: 0x002a=   42; number: -3.1416    ;"},
      {"Jean-Paul", 65535, 299792458,
       "Name: Jean-Paul; answer: 0xffff=65535; number: +2.9979e+08;"}};

  for (auto [name, answer, number, expected_result] : data) {

    std::ostringstream oss;
    auto previous_precision = oss.precision();
    oss << "Name: " << std::setw(9) << name << "; answer: " << std::hex
        << std::showbase << std::internal << std::setfill('0') << std::setw(6)
        << answer << std::noshowbase << std::dec << "=" << std::setfill(' ')
        << std::setw(5) << answer << "; number: " << std::left << std::setw(11)
        << std::setprecision(5) << std::showpos << number << std::noshowpos
        << std::setprecision(previous_precision) << ";";
    std::string result = oss.str();

    std::cout << result << std::endl;
    assert(result == expected_result);
  }
}